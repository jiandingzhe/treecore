#ifndef TREECORE_WIN32_NAMED_PIPE_H
#define TREECORE_WIN32_NAMED_PIPE_H

#include "treecore/NamedPipe.h"

namespace treecore
{
class NamedPipe::Pimpl
{
public:
    Pimpl(const String& pipeName, const bool createPipe);

    ~Pimpl();

    bool connect(const int timeOutMs);

    void disconnectPipe();

    int read(void* destBuffer, const int maxBytesToRead, const int timeOutMilliseconds);

    int write(const void* sourceBuffer, int numBytesToWrite, int timeOutMilliseconds);

    const String filename;
    HANDLE pipeH, cancelEvent;
    bool connected, ownsPipe, shouldStop;
    CriticalSection createFileLock;

private:
    struct OverlappedEvent
    {
        OverlappedEvent()
        {
            zerostruct(over);
            over.hEvent = CreateEvent(0, TRUE, FALSE, 0);
        }

        ~OverlappedEvent()
        {
            CloseHandle(over.hEvent);
        }

        OVERLAPPED over;
    };

    bool waitForIO(OverlappedEvent& over, int timeOutMilliseconds);

    TREECORE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Pimpl)
};
} // namespace treecore

#endif //TREECORE_WIN32_NAMED_PIPE_H
