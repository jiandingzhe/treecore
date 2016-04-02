#include "treecore/native/win32_NamedPipe.h"

#include "treecore/File.h"
#include "treecore/Time.h"

namespace treecore
{

NamedPipe::Pimpl::Pimpl(const String& pipeName, const bool createPipe)
    : filename("\\\\.\\pipe\\" + File::createLegalFileName(pipeName)),
    pipeH(INVALID_HANDLE_VALUE),
    cancelEvent(CreateEvent(0, FALSE, FALSE, 0)),
    connected(false), ownsPipe(createPipe), shouldStop(false)
{
    if (createPipe)
        pipeH = CreateNamedPipe(filename.toWideCharPointer(),
        PIPE_ACCESS_DUPLEX | FILE_FLAG_OVERLAPPED, 0,
        PIPE_UNLIMITED_INSTANCES, 4096, 4096, 0, 0);
}

NamedPipe::Pimpl::~Pimpl()
{
    disconnectPipe();

    if (pipeH != INVALID_HANDLE_VALUE)
        CloseHandle(pipeH);

    CloseHandle(cancelEvent);
}

bool NamedPipe::Pimpl::connect(const int timeOutMs)
{
    if (!ownsPipe)
    {
        if (pipeH != INVALID_HANDLE_VALUE)
            return true;

        const Time timeOutEnd(Time::getCurrentTime() + RelativeTime::milliseconds(timeOutMs));

        for (;;)
        {
            {
                const ScopedLock sl(createFileLock);

                if (pipeH == INVALID_HANDLE_VALUE)
                    pipeH = CreateFile(filename.toWideCharPointer(),
                    GENERIC_READ | GENERIC_WRITE, 0, 0,
                    OPEN_EXISTING, FILE_FLAG_OVERLAPPED, 0);
            }

            if (pipeH != INVALID_HANDLE_VALUE)
                return true;

            if (shouldStop || (timeOutMs >= 0 && Time::getCurrentTime() > timeOutEnd))
                return false;

            Thread::sleep(1);
        }
    }

    if (!connected)
    {
        OverlappedEvent over;

        if (ConnectNamedPipe(pipeH, &over.over) == 0)
        {
            switch (GetLastError())
            {
            case ERROR_PIPE_CONNECTED:   connected = true; break;
            case ERROR_IO_PENDING:
            case ERROR_PIPE_LISTENING:   connected = waitForIO(over, timeOutMs); break;
            default: break;
            }
        }
    }

    return connected;
}

void NamedPipe::Pimpl::disconnectPipe()
{
    if (ownsPipe && connected)
    {
        DisconnectNamedPipe(pipeH);
        connected = false;
    }
}

int NamedPipe::Pimpl::read(void* destBuffer, const int maxBytesToRead, const int timeOutMilliseconds)
{
    while (connect(timeOutMilliseconds))
    {
        if (maxBytesToRead <= 0)
            return 0;

        OverlappedEvent over;
        unsigned long numRead;

        if (ReadFile(pipeH, destBuffer, (DWORD)maxBytesToRead, &numRead, &over.over))
            return (int)numRead;

        const DWORD lastError = GetLastError();

        if (lastError == ERROR_IO_PENDING)
        {
            if (!waitForIO(over, timeOutMilliseconds))
                return -1;

            if (GetOverlappedResult(pipeH, &over.over, &numRead, FALSE))
                return (int)numRead;
        }

        if (ownsPipe && (GetLastError() == ERROR_BROKEN_PIPE || GetLastError() == ERROR_PIPE_NOT_CONNECTED))
            disconnectPipe();
        else
            break;
    }

    return -1;
}

int NamedPipe::Pimpl::write(const void* sourceBuffer, int numBytesToWrite, int timeOutMilliseconds)
{
    if (connect(timeOutMilliseconds))
    {
        if (numBytesToWrite <= 0)
            return 0;

        OverlappedEvent over;
        unsigned long numWritten;

        if (WriteFile(pipeH, sourceBuffer, (DWORD)numBytesToWrite, &numWritten, &over.over))
            return (int)numWritten;

        if (GetLastError() == ERROR_IO_PENDING)
        {
            if (!waitForIO(over, timeOutMilliseconds))
                return -1;

            if (GetOverlappedResult(pipeH, &over.over, &numWritten, FALSE))
                return (int)numWritten;

            if (GetLastError() == ERROR_BROKEN_PIPE && ownsPipe)
                disconnectPipe();
        }
    }

    return -1;
}

bool NamedPipe::Pimpl::waitForIO(OverlappedEvent& over, int timeOutMilliseconds)
{
    if (shouldStop)
        return false;

    HANDLE handles[] = { over.over.hEvent, cancelEvent };
    DWORD waitResult = WaitForMultipleObjects(2, handles, FALSE,
        timeOutMilliseconds >= 0 ? timeOutMilliseconds
        : INFINITE);

    if (waitResult == WAIT_OBJECT_0)
        return true;

    CancelIo(pipeH);
    return false;
}

} // namespace treecore
