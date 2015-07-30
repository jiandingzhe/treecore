#ifndef TREECORE_SYSTEM_STATS_PRIVATE_H
#define TREECORE_SYSTEM_STATS_PRIVATE_H

#include "treecore/Common.h"
#include "treecore/PlatformDefs.h"
#include "treecore/SystemStats.h"

namespace treecore
{

extern SystemStats::CrashHandlerFunction globalCrashHandler;

struct CPUInformation
{
    CPUInformation() NOEXCEPT
        : numCpus (0), hasMMX (false), hasSSE (false),
          hasSSE2 (false), hasSSE3 (false), has3DNow (false)
    {
        initialise();
    }

    void initialise() NOEXCEPT;

    int numCpus;
    bool hasMMX, hasSSE, hasSSE2, hasSSE3, has3DNow;
};

#if defined TREECORE_OS_WINDOWS
static LONG WINAPI handleCrash (LPEXCEPTION_POINTERS)
{
    globalCrashHandler();
    return EXCEPTION_EXECUTE_HANDLER;
}
#else
static void handleCrash (int)
{
    globalCrashHandler();
    kill (getpid(), SIGKILL);
}

int juce_siginterrupt (int sig, int flag);
#endif

} // namespace treecore

#endif // TREECORE_SYSTEM_STATS_PRIVATE_H
