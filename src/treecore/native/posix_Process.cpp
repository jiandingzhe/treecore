#include "treecore/ChildProcess.h"
#include "treecore/InterProcessLock.h"
#include "treecore/Process.h"
#include "treecore/StringArray.h"

#include "treecore/native/posix_ChildProcess.h"
#include "treecore/native/posix_InterProcessLock.h"

#include <sys/ptrace.h>

namespace treecore
{

void TREECORE_STDCALL Process::terminate()
{
#if TREECORE_OS_ANDROID
    _exit( EXIT_FAILURE );
#else
    std::_Exit( EXIT_FAILURE );
#endif
}

int32 TREECORE_STDCALL Process::getProcessID()
{
    return getpid();
}

bool TREECORE_STDCALL Process::isRunningUnderDebugger()
{
    static long testResult = 0;

    if (testResult == 0)
    {
        testResult = ptrace( PT_TRACE_ME, 0, 0, 0 );

        if (testResult >= 0)
        {
            ptrace( PT_DETACH, 0, (caddr_t) 1, 0 );
            testResult = 1;
        }
    }

    return testResult < 0;
}

//
// InterProcessLock
//
InterProcessLock::InterProcessLock ( const String& nm ): name( nm )
{}

InterProcessLock::~InterProcessLock()
{}

bool InterProcessLock::enter( const int timeOutMillisecs )
{
    const ScopedLock sl( lock );

    if (pimpl == nullptr)
    {
        pimpl = new Pimpl( name, timeOutMillisecs );

        if (pimpl->handle == 0)
            pimpl = nullptr;
    }
    else
    {
        pimpl->refCount++;
    }

    return pimpl != nullptr;
}

void InterProcessLock::exit()
{
    const ScopedLock sl( lock );

    // Trying to release the lock too many times!
    treecore_assert( pimpl != nullptr );

    if (pimpl != nullptr && --(pimpl->refCount) == 0)
        pimpl = nullptr;
}

//
// ChildProcess
//
bool ChildProcess::start( const String& command, int streamFlags )
{
    return start( StringArray::fromTokens( command, true ), streamFlags );
}

bool ChildProcess::start( const StringArray& args, int streamFlags )
{
    if (args.size() == 0)
        return false;

    activeProcess = new ActiveProcess( args, streamFlags );

    if (activeProcess->childPID == 0)
        activeProcess = nullptr;

    return activeProcess != nullptr;
}

} // namespace treecore
