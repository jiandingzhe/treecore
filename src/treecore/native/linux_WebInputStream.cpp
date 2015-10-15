#include "treecore/WebInputStream.h"

#include "treecore/MemoryBlock.h"
#include "treecore/MemoryOutputStream.h"
#include "treecore/Time.h"
#include "treecore/IntTypes.h"

namespace treecore
{

typedef HashMap<String, String> StrMap;

struct WebInputStream::Impl
{
    Impl(bool is_post, int time_out, int num_redirect)
        : isPost(is_post)
        , timeOutMs(time_out)
        , numRedirectsToFollow(num_redirect)
    {}

    int socketHandle;
    int levelsOfRedirection;
    StringArray headerLines;
    String address;
    String headers;
    MemoryBlock postData;
    int64 position;
    bool finished;

    const bool isPost;
    const int timeOutMs;
    const int numRedirectsToFollow;
};


WebInputStream::WebInputStream (const String& address_,
                                bool isPost_,
                                const MemoryBlock& postData_,
                                URL::OpenStreamProgressCallback* progressCallback,
                                void* progressCallbackContext,
                                const String& headers_,
                                int timeOutMs_,
                                HashMap<String, String>* responseHeaders,
                                const int maxRedirects)
    : m_status_code(0)
    , m_impl(new WebInputStream::Impl(isPost_, timeOutMs_, maxRedirects))
{
    m_status_code = createConnection (progressCallback, progressCallbackContext, m_impl->numRedirectsToFollow);
    m_impl->socketHandle = -1;
    m_impl->levelsOfRedirection = 0;
    m_impl->address = address_;
    m_impl->headers = headers_;
    m_impl->postData = postData_;
    m_impl->position = 0;
    m_impl->finished = false;

    if (responseHeaders != nullptr && ! isError())
    {
        for (int i = 0; i < m_impl->headerLines.size(); ++i)
        {
            const String& headersEntry = m_impl->headerLines[i];
            const String key (headersEntry.upToFirstOccurrenceOf (": ", false, false));
            const String value (headersEntry.fromFirstOccurrenceOf (": ", false, false));

            StrMap::Iterator i_resp(*responseHeaders);
            if (!responseHeaders->insertOrSelect(key, value, i_resp))
            {
                i_resp.value() += "," + value;
            }
        }
    }
}

WebInputStream::~WebInputStream()
{
    closeSocket();
}

//==============================================================================
bool WebInputStream::isError() const
{
    return m_impl->socketHandle < 0;
}

bool WebInputStream::isExhausted()
{
    return m_impl->finished;
}

int64 WebInputStream::getPosition()
{
    return m_impl->position;
}

int64 WebInputStream::getTotalLength()
{
    //xxx to do
    return -1;
}

int WebInputStream::read (void* buffer, int bytesToRead)
{
    if (m_impl->finished || isError())
        return 0;

    fd_set readbits;
    FD_ZERO (&readbits);
    FD_SET (m_impl->socketHandle, &readbits);

    struct timeval tv;
    tv.tv_sec = jmax (1, m_impl->timeOutMs / 1000);
    tv.tv_usec = 0;

    if (select (m_impl->socketHandle + 1, &readbits, 0, 0, &tv) <= 0)
        return 0;   // (timeout)

    const int bytesRead = jmax (0, (int) recv (m_impl->socketHandle, buffer, bytesToRead, MSG_WAITALL));
    if (bytesRead == 0)
        m_impl->finished = true;

    m_impl->position += bytesRead;
    return bytesRead;
}

bool WebInputStream::setPosition (int64 wantedPos)
{
    if (isError())
        return false;

    if (wantedPos != m_impl->position)
    {
        m_impl->finished = false;

        if (wantedPos < m_impl->position)
        {
            closeSocket();
            m_impl->position = 0;
            m_status_code = createConnection (0, 0, m_impl->numRedirectsToFollow);
        }

        skipNextBytes (wantedPos - m_impl->position);
    }

    return true;
}

//==============================================================================


void WebInputStream::closeSocket (bool resetLevelsOfRedirection)
{
    if (m_impl->socketHandle >= 0)
        close (m_impl->socketHandle);

    m_impl->socketHandle = -1;
    if (resetLevelsOfRedirection)
        m_impl->levelsOfRedirection = 0;
}

int WebInputStream::createConnection (URL::OpenStreamProgressCallback* progressCallback,
                                      void* progressCallbackContext,
                                      const int numRedirects)
{
    closeSocket (false);

    uint32 timeOutTime = Time::getMillisecondCounter();

    if (m_impl->timeOutMs == 0)
        timeOutTime += 30000;
    else if (m_impl->timeOutMs < 0)
        timeOutTime = 0xffffffff;
    else
        timeOutTime += m_impl->timeOutMs;

    String hostName, hostPath;
    int hostPort;
    if (! decomposeURL (m_impl->address, hostName, hostPath, hostPort))
        return 0;

    String serverName, proxyName, proxyPath;
    int proxyPort = 0;
    int port = 0;

    const String proxyURL (getenv ("http_proxy"));
    if (proxyURL.startsWithIgnoreCase ("http://"))
    {
        if (! decomposeURL (proxyURL, proxyName, proxyPath, proxyPort))
            return 0;

        serverName = proxyName;
        port = proxyPort;
    }
    else
    {
        serverName = hostName;
        port = hostPort;
    }

    struct addrinfo hints;
    zerostruct (hints);

    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_NUMERICSERV;

    struct addrinfo* result = nullptr;
    if (getaddrinfo (serverName.toUTF8(), String (port).toUTF8(), &hints, &result) != 0 || result == 0)
        return 0;

    m_impl->socketHandle = socket (result->ai_family, result->ai_socktype, 0);

    if (m_impl->socketHandle == -1)
    {
        freeaddrinfo (result);
        return 0;
    }

    int receiveBufferSize = 16384;
    setsockopt (m_impl->socketHandle, SOL_SOCKET, SO_RCVBUF, (char*) &receiveBufferSize, sizeof (receiveBufferSize));
    setsockopt (m_impl->socketHandle, SOL_SOCKET, SO_KEEPALIVE, 0, 0);

#if defined TREECORE_OS_OSX
    setsockopt (m_impl->socketHandle, SOL_SOCKET, SO_NOSIGPIPE, 0, 0);
#endif

    if (connect (m_impl->socketHandle, result->ai_addr, result->ai_addrlen) == -1)
    {
        closeSocket();
        freeaddrinfo (result);
        return 0;
    }

    freeaddrinfo (result);

    {
        const MemoryBlock requestHeader (createRequestHeader (hostName, hostPort, proxyName, proxyPort,
                                                              hostPath, m_impl->address, m_impl->headers, m_impl->postData, m_impl->isPost));

        if (! sendHeader (m_impl->socketHandle, requestHeader, timeOutTime,
                          progressCallback, progressCallbackContext))
        {
            closeSocket();
            return 0;
        }
    }

    String responseHeader (readResponse (timeOutTime));
    m_impl->position = 0;

    if (responseHeader.isNotEmpty())
    {
        m_impl->headerLines = StringArray::fromLines (responseHeader);

        const int status = responseHeader.fromFirstOccurrenceOf (" ", false, false)
                .substring (0, 3).getIntValue();

        String location (findHeaderItem (m_impl->headerLines, "Location:"));

        if (++m_impl->levelsOfRedirection <= numRedirects
                && status >= 300 && status < 400
                && location.isNotEmpty() && location != m_impl->address)
        {
            if (! (location.startsWithIgnoreCase ("http://")
                   || location.startsWithIgnoreCase ("https://")
                   || location.startsWithIgnoreCase ("ftp://")))
            {
                // The following is a bit dodgy. Ideally, we should do a proper transform of the relative URI to a target URI
                if (location.startsWithChar ('/'))
                    location = URL(m_impl->address).withNewSubPath (location).toString (true);
                else
                    location = m_impl->address + "/" + location;
            }

            m_impl->address = location;
            return createConnection (progressCallback, progressCallbackContext, numRedirects);
        }

        return status;
    }

    closeSocket();
    return 0;
}

//==============================================================================
String WebInputStream::readResponse (const uint32 timeOutTime)
{
    int numConsecutiveLFs  = 0;
    MemoryOutputStream buffer;

    while (numConsecutiveLFs < 2
           && buffer.getDataSize() < 32768
           && Time::getMillisecondCounter() <= timeOutTime
           && ! (m_impl->finished || isError()))
    {
        char c = 0;
        if (read (&c, 1) != 1)
            return String();

        buffer.writeByte (c);

        if (c == '\n')
            ++numConsecutiveLFs;
        else if (c != '\r')
            numConsecutiveLFs = 0;
    }

    const String header (buffer.toString().trimEnd());

    if (header.startsWithIgnoreCase ("HTTP/"))
        return header;

    return String();
}

void WebInputStream::writeValueIfNotPresent (MemoryOutputStream& dest, const String& headers, const String& key, const String& value)
{
    if (! headers.containsIgnoreCase (key))
        dest << "\r\n" << key << ' ' << value;
}

void WebInputStream::writeHost (MemoryOutputStream& dest, const bool isPost,
                                const String& path, const String& host, int /*port*/)
{
    dest << (isPost ? "POST " : "GET ") << path << " HTTP/1.0\r\nHost: " << host;
}

MemoryBlock WebInputStream::createRequestHeader (const String& hostName, const int hostPort,
                                                 const String& proxyName, const int proxyPort,
                                                 const String& hostPath, const String& originalURL,
                                                 const String& userHeaders, const MemoryBlock& postData,
                                                 const bool isPost)
{
    MemoryOutputStream header;

    if (proxyName.isEmpty())
        writeHost (header, isPost, hostPath, hostName, hostPort);
    else
        writeHost (header, isPost, originalURL, proxyName, proxyPort);

    writeValueIfNotPresent (header, userHeaders, "User-Agent:", "JUCE/" JUCE_STRINGIFY(JUCE_MAJOR_VERSION)
                            "." JUCE_STRINGIFY(JUCE_MINOR_VERSION)
                            "." JUCE_STRINGIFY(JUCE_BUILDNUMBER));
    writeValueIfNotPresent (header, userHeaders, "Connection:", "close");

    if (isPost)
    {
        writeValueIfNotPresent (header, userHeaders, "Content-Length:", String ((int) postData.getSize()));
        header << userHeaders << "\r\n" << postData;
    }
    else
    {
        header << "\r\n" << userHeaders << "\r\n";
    }

    return header.getMemoryBlock();
}

bool WebInputStream::sendHeader (int socketHandle, const MemoryBlock& requestHeader, const uint32 timeOutTime,
                                 URL::OpenStreamProgressCallback* progressCallback, void* progressCallbackContext)
{
    size_t totalHeaderSent = 0;

    while (totalHeaderSent < requestHeader.getSize())
    {
        if (Time::getMillisecondCounter() > timeOutTime)
            return false;

        const int numToSend = jmin (1024, (int) (requestHeader.getSize() - totalHeaderSent));

        if (send (socketHandle, static_cast <const char*> (requestHeader.getData()) + totalHeaderSent, numToSend, 0) != numToSend)
            return false;

        totalHeaderSent += numToSend;

        if (progressCallback != nullptr && ! progressCallback (progressCallbackContext, totalHeaderSent, requestHeader.getSize()))
            return false;
    }

    return true;
}

bool WebInputStream::decomposeURL (const String& url, String& host, String& path, int& port)
{
    if (! url.startsWithIgnoreCase ("http://"))
        return false;

    const int nextSlash = url.indexOfChar (7, '/');
    int nextColon = url.indexOfChar (7, ':');
    if (nextColon > nextSlash && nextSlash > 0)
        nextColon = -1;

    if (nextColon >= 0)
    {
        host = url.substring (7, nextColon);

        if (nextSlash >= 0)
            port = url.substring (nextColon + 1, nextSlash).getIntValue();
        else
            port = url.substring (nextColon + 1).getIntValue();
    }
    else
    {
        port = 80;

        if (nextSlash >= 0)
            host = url.substring (7, nextSlash);
        else
            host = url.substring (7);
    }

    if (nextSlash >= 0)
        path = url.substring (nextSlash);
    else
        path = "/";

    return true;
}

String WebInputStream::findHeaderItem (const StringArray& lines, const String& itemName)
{
    for (int i = 0; i < lines.size(); ++i)
        if (lines[i].startsWithIgnoreCase (itemName))
            return lines[i].substring (itemName.length()).trim();

    return String();
}

} // namespace treecore
