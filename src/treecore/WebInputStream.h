#ifndef TREECORE_WEB_INPUT_STREAM
#define TREECORE_WEB_INPUT_STREAM

#include "treecore/URL.h"
#include "treecore/HashMap.h"
#include "treecore/InputStream.h"

namespace treecore
{

class MemoryBlock;
class MemoryOutputStream;

class WebInputStream: public InputStream
{
public:
    WebInputStream (const String&                    address_,
                    bool                             isPost_,
                    const                            MemoryBlock& postData_,
                    URL::OpenStreamProgressCallback* progressCallback,
                    void*                            progressCallbackContext,
                    const String&                    headers_,
                    int                              timeOutMs_,
                    HashMap<String,String>*          responseHeaders,
                    const int                        maxRedirects);

    virtual ~WebInputStream();

    bool isError() const;
    bool isExhausted() override;
    int64 getPosition() override;
    int64 getTotalLength() override;
    int read (void* buffer, int bytesToRead) override;
    bool setPosition (int64 wantedPos) override;
    void closeSocket (bool resetLevelsOfRedirection = true);

    int createConnection (URL::OpenStreamProgressCallback* progressCallback, void* progressCallbackContext, const int numRedirects);

    String readResponse (const uint32 timeOutTime);

    static void writeValueIfNotPresent (MemoryOutputStream& dest,
                                        const String& headers,
                                        const String& key,
                                        const String& value);

    static void writeHost (MemoryOutputStream& dest,
                           const bool isPost,
                           const String& path,
                           const String& host,
                           int port);

    static MemoryBlock createRequestHeader (const String& hostName, const int hostPort,
                                            const String& proxyName, const int proxyPort,
                                            const String& hostPath, const String& originalURL,
                                            const String& userHeaders, const MemoryBlock& postData,
                                            const bool isPost);

    static bool sendHeader (int socketHandle,
                            const MemoryBlock& requestHeader,
                            const uint32 timeOutTime,
                            URL::OpenStreamProgressCallback* progressCallback,
                            void* progressCallbackContext);

    static bool decomposeURL (const String& url, String& host, String& path, int& port);

    static String findHeaderItem (const StringArray& lines, const String& itemName);

    int getStatus() const noexcept
    {
        return m_status_code;
    }

private:

#if defined TREECORE_OS_WINDOWS
    void close();
    void createConnection(URL::OpenStreamProgressCallback* progressCallback, void* progressCallbackContext);
    void openConnection(URL_COMPONENTS& uc, HINTERNET sessionHandle, URL::OpenStreamProgressCallback* progressCallback, void* progressCallbackContext);
    void applyTimeout(HINTERNET sessionHandle, const DWORD option);
    void openHTTPConnection(URL_COMPONENTS& uc, URL::OpenStreamProgressCallback* progressCallback, void* progressCallbackContext);
#endif

    void setSecurityFlags();

protected:
    struct Impl;
    int m_status_code;
    ScopedPointer<Impl> m_impl;

    TREECORE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (WebInputStream)
};

} // namespace treecore

#endif // TREECORE_WEB_INPUT_STREAM
