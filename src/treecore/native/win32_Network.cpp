/*
  ==============================================================================

   This file is part of the juce_core module of the JUCE library.
   Copyright (c) 2013 - Raw Material Software Ltd.

   Permission to use, copy, modify, and/or distribute this software for any purpose with
   or without fee is hereby granted, provided that the above copyright notice and this
   permission notice appear in all copies.

   THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH REGARD
   TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS. IN
   NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL
   DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER
   IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN
   CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.

   ------------------------------------------------------------------------------

   NOTE! This permissive ISC license applies ONLY to files within the juce_core module!
   All other JUCE modules are covered by a dual GPL/commercial license, so if you are
   using any other modules, be sure to check that you also comply with their license.

   For more details, visit www.juce.com

  ==============================================================================
*/

#ifndef INTERNET_FLAG_NEED_FILE
 #define INTERNET_FLAG_NEED_FILE 0x00000010
#endif

#ifndef INTERNET_OPTION_DISABLE_AUTODIAL
 #define INTERNET_OPTION_DISABLE_AUTODIAL 70
#endif

#include "treecore/WebInputStream.h"
#include "treecore/HashMap.h"
#include "treecore/MacAddress.h"
#include "treecore/URL.h"
#include "treecore/IPAddress.h"
#include "treecore/DynamicLibrary.h"
#include "treecore/MemoryBlock.h"
#include "treecore/Process.h"

namespace treecore
{

struct WebInputStream::Impl
{
    HINTERNET connection, request;
    String address, headers;
    MemoryBlock postData;
    int64 position;
    bool finished;
    const bool isPost;
    int timeOutMs;
};


WebInputStream::WebInputStream(const String& address_, bool isPost_, const MemoryBlock& postData_,
                URL::OpenStreamProgressCallback* progressCallback, void* progressCallbackContext,
                const String& headers_, int timeOutMs_, HashMap<String, String>* responseHeaders, int numRedirectsToFollow)
    : m_status_code(0)
    , m_impl(new Impl{ 0, 0, address_, headers_, postData_, 0, false, isPost_, timeOutMs_})
{
    while (numRedirectsToFollow-- >= 0)
    {
        createConnection (progressCallback, progressCallbackContext);

        if (! isError())
        {
            DWORD bufferSizeBytes = 4096;
            HashMap<String, String> headers;

            for (;;)
            {
                HeapBlock<char> buffer ((size_t) bufferSizeBytes);

                if (HttpQueryInfo(m_impl->request, HTTP_QUERY_RAW_HEADERS_CRLF, buffer.getData(), &bufferSizeBytes, 0))
                {
                    StringArray headersArray;
                    headersArray.addLines (String (reinterpret_cast<const WCHAR*> (buffer.getData())));

                    for (int i = 0; i < headersArray.size(); ++i)
                    {
                        const String& header = headersArray[i];
                        const String key   (header.upToFirstOccurrenceOf (": ", false, false));
                        const String value (header.fromFirstOccurrenceOf (": ", false, false));
                        const String previousValue (headers[key]);
                        headers.set (key, previousValue.isEmpty() ? value : (previousValue + "," + value));
                    }

                    break;
                }

                if (GetLastError() != ERROR_INSUFFICIENT_BUFFER)
                    break;

                bufferSizeBytes += 4096;
            }

            DWORD status = 0;
            DWORD statusSize = sizeof (status);

            if (HttpQueryInfo(m_impl->request, HTTP_QUERY_STATUS_CODE | HTTP_QUERY_FLAG_NUMBER, &status, &statusSize, 0))
            {
                m_status_code = (int) status;

                if (numRedirectsToFollow >= 0
                    && (m_status_code == 301 || m_status_code == 302 || m_status_code == 303 || m_status_code == 307))
                {
                    String newLocation (headers["Location"]);

                    // Check whether location is a relative URI - this is an incomplete test for relative path,
                    // but we'll use it for now (valid protocols for this implementation are http, https & ftp)
                    if (! (newLocation.startsWithIgnoreCase ("http://")
                            || newLocation.startsWithIgnoreCase ("https://")
                            || newLocation.startsWithIgnoreCase ("ftp://")))
                    {
                        if (newLocation.startsWithChar ('/'))
                            newLocation = URL (m_impl->address).withNewSubPath (newLocation).toString (true);
                        else
                            newLocation = m_impl->address + "/" + newLocation;
                    }

                    if (newLocation.isNotEmpty() && newLocation != m_impl->address)
                    {
                        m_impl->address = newLocation;
                        continue;
                    }
                }
            }

            if (responseHeaders != nullptr)
            {
                HashMap<String, String>::ConstIterator i(headers);
                while (i.next())
                {
                    responseHeaders->set(i.key(), i.value());
                }
            }
        }

        break;
    }
}

WebInputStream::~WebInputStream()
{
    close();
}

    //==============================================================================
bool WebInputStream::isError() const        { return m_impl->request == 0; }
bool WebInputStream::isExhausted()          { return m_impl->finished; }
int64 WebInputStream::getPosition()         { return m_impl->position; }

int64 WebInputStream::getTotalLength()
{
    if (! isError())
    {
        DWORD index = 0, result = 0, size = sizeof (result);

        if (HttpQueryInfo(m_impl->request, HTTP_QUERY_CONTENT_LENGTH | HTTP_QUERY_FLAG_NUMBER, &result, &size, &index))
            return (int64) result;
    }

    return -1;
}

int WebInputStream::read(void* buffer, int bytesToRead)
{
    jassert (buffer != nullptr && bytesToRead >= 0);
    DWORD bytesRead = 0;

    if (!(m_impl->finished || isError()))
    {
        InternetReadFile(m_impl->request, buffer, (DWORD)bytesToRead, &bytesRead);
        m_impl->position += bytesRead;

        if (bytesRead == 0)
            m_impl->finished = true;
    }

    return (int) bytesRead;
}

bool WebInputStream::setPosition(int64 wantedPos)
{
    if (isError())
        return false;

    if (wantedPos != m_impl->position)
    {
        m_impl->finished = false;
        m_impl->position = (int64)InternetSetFilePointer(m_impl->request, (LONG)wantedPos, 0, FILE_BEGIN, 0);

        if (m_impl->position == wantedPos)
            return true;

        if (wantedPos < m_impl->position)
        {
            close();
            m_impl->position = 0;
            createConnection (0, 0);
        }

        skipNextBytes(wantedPos - m_impl->position);
    }

    return true;
}
    

void WebInputStream::close()
{
    if (m_impl->request != 0)
    {
        InternetCloseHandle(m_impl->request);
        m_impl->request = 0;
    }

    if (m_impl->connection != 0)
    {
        InternetCloseHandle(m_impl->connection);
        m_impl->connection = 0;
    }
}

void WebInputStream::createConnection(URL::OpenStreamProgressCallback* progressCallback,
                        void* progressCallbackContext)
{
    static HINTERNET sessionHandle = InternetOpen (_T("treecore"), INTERNET_OPEN_TYPE_PRECONFIG, 0, 0, 0);

    close();

    if (sessionHandle != 0)
    {
        // break up the url..
        const int fileNumChars = 65536;
        const int serverNumChars = 2048;
        const int usernameNumChars = 1024;
        const int passwordNumChars = 1024;
        HeapBlock<TCHAR> file (fileNumChars), server (serverNumChars),
                            username (usernameNumChars), password (passwordNumChars);

        URL_COMPONENTS uc = { 0 };
        uc.dwStructSize = sizeof (uc);
        uc.lpszUrlPath = file;
        uc.dwUrlPathLength = fileNumChars;
        uc.lpszHostName = server;
        uc.dwHostNameLength = serverNumChars;
        uc.lpszUserName = username;
        uc.dwUserNameLength = usernameNumChars;
        uc.lpszPassword = password;
        uc.dwPasswordLength = passwordNumChars;

        if (InternetCrackUrl(m_impl->address.toWideCharPointer(), 0, 0, &uc))
            openConnection (uc, sessionHandle, progressCallback, progressCallbackContext);
    }
}

void WebInputStream::openConnection(URL_COMPONENTS& uc, HINTERNET sessionHandle,
                        URL::OpenStreamProgressCallback* progressCallback,
                        void* progressCallbackContext)
{
    int disable = 1;
    InternetSetOption (sessionHandle, INTERNET_OPTION_DISABLE_AUTODIAL, &disable, sizeof (disable));

    if (m_impl->timeOutMs == 0)
        m_impl->timeOutMs = 30000;
    else if (m_impl->timeOutMs < 0)
        m_impl->timeOutMs = -1;

    applyTimeout (sessionHandle, INTERNET_OPTION_CONNECT_TIMEOUT);
    applyTimeout (sessionHandle, INTERNET_OPTION_RECEIVE_TIMEOUT);
    applyTimeout (sessionHandle, INTERNET_OPTION_SEND_TIMEOUT);
    applyTimeout (sessionHandle, INTERNET_OPTION_DATA_RECEIVE_TIMEOUT);
    applyTimeout (sessionHandle, INTERNET_OPTION_DATA_SEND_TIMEOUT);

    const bool isFtp = m_impl->address.startsWithIgnoreCase("ftp:");

    m_impl->connection = InternetConnect(sessionHandle, uc.lpszHostName, uc.nPort,
                                    uc.lpszUserName, uc.lpszPassword,
                                    isFtp ? (DWORD) INTERNET_SERVICE_FTP
                                        : (DWORD) INTERNET_SERVICE_HTTP,
                                    0, 0);
    if (m_impl->connection != 0)
    {
        if (isFtp)
            m_impl->request = FtpOpenFile(m_impl->connection, uc.lpszUrlPath, GENERIC_READ,
                                    FTP_TRANSFER_TYPE_BINARY | INTERNET_FLAG_NEED_FILE, 0);
        else
            openHTTPConnection (uc, progressCallback, progressCallbackContext);
    }
}

void WebInputStream::applyTimeout(HINTERNET sessionHandle, const DWORD option)
{
    InternetSetOption(sessionHandle, option, &m_impl->timeOutMs, sizeof(m_impl->timeOutMs));
}

void WebInputStream::openHTTPConnection(URL_COMPONENTS& uc, URL::OpenStreamProgressCallback* progressCallback,
                            void* progressCallbackContext)
{
    const TCHAR* mimeTypes[] = { _T("*/*"), nullptr };

    DWORD flags = INTERNET_FLAG_RELOAD | INTERNET_FLAG_NO_CACHE_WRITE | INTERNET_FLAG_NO_COOKIES
                    | INTERNET_FLAG_NO_AUTO_REDIRECT | SECURITY_SET_MASK;

    if (m_impl->address.startsWithIgnoreCase("https:"))
        flags |= INTERNET_FLAG_SECURE;  // (this flag only seems necessary if the OS is running IE6 -
                                        //  IE7 seems to automatically work out when it's https)

    m_impl->request = HttpOpenRequest(m_impl->connection, m_impl->isPost ? _T("POST") : _T("GET"),
                                uc.lpszUrlPath, 0, 0, mimeTypes, flags, 0);

    if (m_impl->request != 0)
    {
        setSecurityFlags();

        INTERNET_BUFFERS buffers = { 0 };
        buffers.dwStructSize = sizeof (INTERNET_BUFFERS);
        buffers.lpcszHeader = m_impl->headers.toWideCharPointer();
        buffers.dwHeadersLength = (DWORD)m_impl->headers.length();
        buffers.dwBufferTotal = (DWORD)m_impl->postData.getSize();

        if (HttpSendRequestEx(m_impl->request, &buffers, 0, HSR_INITIATE, 0))
        {
            int bytesSent = 0;

            for (;;)
            {
                const int bytesToDo = treecore::jmin(1024, (int)m_impl->postData.getSize() - bytesSent);
                DWORD bytesDone = 0;

                if (bytesToDo > 0
                    && !InternetWriteFile(m_impl->request,
                    static_cast<const char*> (m_impl->postData.getData()) + bytesSent,
                                                (DWORD) bytesToDo, &bytesDone))
                {
                    break;
                }

                if (bytesToDo == 0 || (int) bytesDone < bytesToDo)
                {
                    if (HttpEndRequest(m_impl->request, 0, 0, 0))
                        return;

                    break;
                }

                bytesSent += bytesDone;

                if (progressCallback != nullptr
                    && !progressCallback(progressCallbackContext, bytesSent, (int)m_impl->postData.getSize()))
                    break;
            }
        }
    }

    close();
}

void WebInputStream::setSecurityFlags()
{
    DWORD dwFlags = 0, dwBuffLen = sizeof (DWORD);
    InternetQueryOption(m_impl->request, INTERNET_OPTION_SECURITY_FLAGS, &dwFlags, &dwBuffLen);
    dwFlags |= SECURITY_FLAG_IGNORE_UNKNOWN_CA | SECURITY_SET_MASK;
    InternetSetOption(m_impl->request, INTERNET_OPTION_SECURITY_FLAGS, &dwFlags, sizeof(dwFlags));
}


//==============================================================================
struct GetAdaptersInfoHelper
{
    bool callGetAdaptersInfo()
    {
        DynamicLibrary dll ("iphlpapi.dll");
        TREECORE_LOAD_WINAPI_FUNCTION (dll, GetAdaptersInfo, getAdaptersInfo, DWORD, (PIP_ADAPTER_INFO, PULONG))

        if (getAdaptersInfo == nullptr)
            return false;

        adapterInfo.malloc (1);
        ULONG len = sizeof (IP_ADAPTER_INFO);

        if (getAdaptersInfo (adapterInfo, &len) == ERROR_BUFFER_OVERFLOW)
            adapterInfo.malloc (len, 1);

        return getAdaptersInfo (adapterInfo, &len) == NO_ERROR;
    }

    HeapBlock<IP_ADAPTER_INFO> adapterInfo;
};

namespace MACAddressHelpers
{
    static void addAddress(Array<MacAddress>& result, const MacAddress& ma)
    {
        if (! ma.isNull())
            result.addIfNotAlreadyThere (ma);
    }

    static void getViaGetAdaptersInfo(Array<MacAddress>& result)
    {
        GetAdaptersInfoHelper gah;

        if (gah.callGetAdaptersInfo())
        {
            for (PIP_ADAPTER_INFO adapter = gah.adapterInfo; adapter != nullptr; adapter = adapter->Next)
                if (adapter->AddressLength >= 6)
                    addAddress(result, MacAddress(adapter->Address));
        }
    }

    static void getViaNetBios(Array<MacAddress>& result)
    {
        DynamicLibrary dll ("netapi32.dll");
        TREECORE_LOAD_WINAPI_FUNCTION (dll, Netbios, NetbiosCall, UCHAR, (PNCB))

        if (NetbiosCall != 0)
        {
            LANA_ENUM enums = { 0 };

            {
                NCB ncb = { 0 };
                ncb.ncb_command = NCBENUM;
                ncb.ncb_buffer = (unsigned char*) &enums;
                ncb.ncb_length = sizeof (LANA_ENUM);
                NetbiosCall (&ncb);
            }

            for (int i = 0; i < enums.length; ++i)
            {
                NCB ncb2 = { 0 };
                ncb2.ncb_command = NCBRESET;
                ncb2.ncb_lana_num = enums.lana[i];

                if (NetbiosCall (&ncb2) == 0)
                {
                    NCB ncb = { 0 };
                    memcpy (ncb.ncb_callname, "*                   ", NCBNAMSZ);
                    ncb.ncb_command = NCBASTAT;
                    ncb.ncb_lana_num = enums.lana[i];

                    struct ASTAT
                    {
                        ADAPTER_STATUS adapt;
                        NAME_BUFFER    NameBuff [30];
                    };

                    ASTAT astat;
                    zerostruct (astat);
                    ncb.ncb_buffer = (unsigned char*) &astat;
                    ncb.ncb_length = sizeof (ASTAT);

                    if (NetbiosCall (&ncb) == 0 && astat.adapt.adapter_type == 0xfe)
                        addAddress(result, MacAddress(astat.adapt.adapter_address));
                }
            }
        }
    }
}

void MacAddress::findAllAddresses(Array<MacAddress>& result)
{
    MACAddressHelpers::getViaGetAdaptersInfo (result);
    MACAddressHelpers::getViaNetBios (result);
}

void IPAddress::findAllAddresses (Array<IPAddress>& result)
{
    result.addIfNotAlreadyThere (IPAddress::local());

    GetAdaptersInfoHelper gah;

    if (gah.callGetAdaptersInfo())
    {
        for (PIP_ADAPTER_INFO adapter = gah.adapterInfo; adapter != nullptr; adapter = adapter->Next)
        {
            IPAddress ip (adapter->IpAddressList.IpAddress.String);

            if (ip != IPAddress::any())
                result.addIfNotAlreadyThere (ip);
        }
    }
}

//==============================================================================
bool TREECORE_STDCALL Process::openEmailWithAttachments (const String& targetEmailAddress,
                                                      const String& emailSubject,
                                                      const String& bodyText,
                                                      const StringArray& filesToAttach)
{
    DynamicLibrary dll ("MAPI32.dll");
    TREECORE_LOAD_WINAPI_FUNCTION (dll, MAPISendMail, mapiSendMail,
                               ULONG, (LHANDLE, ULONG, lpMapiMessage, ::FLAGS, ULONG))

    if (mapiSendMail == nullptr)
        return false;

    MapiMessage message = { 0 };
    message.lpszSubject = (LPSTR) emailSubject.toRawUTF8();
    message.lpszNoteText = (LPSTR) bodyText.toRawUTF8();

    MapiRecipDesc recip = { 0 };
    recip.ulRecipClass = MAPI_TO;
    String targetEmailAddress_ (targetEmailAddress);
    if (targetEmailAddress_.isEmpty())
        targetEmailAddress_ = " "; // (Windows Mail can't deal with a blank address)
    recip.lpszName = (LPSTR) targetEmailAddress_.toRawUTF8();
    message.nRecipCount = 1;
    message.lpRecips = &recip;

    HeapBlock <MapiFileDesc> files;
    files.calloc ((size_t) filesToAttach.size());

    message.nFileCount = (ULONG) filesToAttach.size();
    message.lpFiles = files;

    for (int i = 0; i < filesToAttach.size(); ++i)
    {
        files[i].nPosition = (ULONG) -1;
        files[i].lpszPathName = (LPSTR) filesToAttach[i].toRawUTF8();
    }

    return mapiSendMail (0, 0, &message, MAPI_DIALOG | MAPI_LOGON_UI, 0) == SUCCESS_SUCCESS;
}

} // namespace treecore
