#ifndef TREEJUCE_SYSTEM_STATS_PRIVATE_H
#define TREEJUCE_SYSTEM_STATS_PRIVATE_H

#include "treejuce/Common.h"
#include "treejuce/PlatformDefs.h"
#include "treejuce/SystemStats.h"

TREEFACE_JUCE_NAMESPACE_BEGIN

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

#if defined TREEJUCE_OS_WINDOWS
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

TREEFACE_JUCE_NAMESPACE_END

#endif // TREEJUCE_SYSTEM_STATS_PRIVATE_H
