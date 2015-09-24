#ifndef TREECORE_NATIVE_WIN32_CHILD_PROCESS_H
#define TREECORE_NATIVE_WIN32_CHILD_PROCESS_H

#include "treecore/ChildProcess.h"
#include "treecore/BasicNativeHeaders.h"

namespace treecore
{

class ChildProcess::ActiveProcess
{
public:
    ActiveProcess (const String& command, int streamFlags);

    ~ActiveProcess();

    bool isRunning() const noexcept
    {
        return WaitForSingleObject (processInfo.hProcess, 0) != WAIT_OBJECT_0;
    }

    int read (void* dest, int numNeeded) const noexcept;

    bool killProcess() const noexcept
    {
        return TerminateProcess (processInfo.hProcess, 0) != FALSE;
    }

    uint32 getExitCode() const noexcept;

    bool ok;

private:
    HANDLE readPipe, writePipe;
    PROCESS_INFORMATION processInfo;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ActiveProcess)
};

} // namespace treecore

#endif // TREECORE_NATIVE_WIN32_CHILD_PROCESS_H
