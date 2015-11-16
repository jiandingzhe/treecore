#ifndef TREECORE_POSIX_NAMED_PIPE_H
#define TREECORE_POSIX_NAMED_PIPE_H

#include "treecore/NamedPipe.h"
#include "treecore/Time.h"

namespace treecore
{

class NamedPipe::Pimpl
{
public:
    Pimpl (const String& pipePath, bool createPipe);

    ~Pimpl();

    int read (char* destBuffer, int maxBytesToRead, int timeOutMilliseconds);

    int write (const char* sourceBuffer, int numBytesToWrite, int timeOutMilliseconds);

    bool createFifos() const
    {
        return (mkfifo (pipeInName .toUTF8(), 0666) == 0 || errno == EEXIST)
            && (mkfifo (pipeOutName.toUTF8(), 0666) == 0 || errno == EEXIST);
    }

    const String pipeInName, pipeOutName;
    int pipeIn, pipeOut;

    const bool createdPipe;
    bool stopReadOperation;

private:
    static void signalHandler (int) {}

    static uint32 getTimeoutEnd (const int timeOutMilliseconds)
    {
        return timeOutMilliseconds >= 0 ? Time::getMillisecondCounter() + (uint32) timeOutMilliseconds : 0;
    }

    static bool hasExpired (const uint32 timeoutEnd)
    {
        return timeoutEnd != 0 && Time::getMillisecondCounter() >= timeoutEnd;
    }

    int openPipe (const String& name, int flags, const uint32 timeoutEnd);

    static void waitForInput (const int handle, const int timeoutMsecs) noexcept;

    TREECORE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Pimpl)
};

} // namespace treecore

#endif // TREECORE_POSIX_NAMED_PIPE_H
