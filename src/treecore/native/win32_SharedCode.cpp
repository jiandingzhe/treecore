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

#include "treecore/CriticalSection.h"
#include "treecore/DynamicLibrary.h"
#include "treecore/HighResolutionTimer.h"
#include "treecore/Process.h"
#include "treecore/StringArray.h"
#include "treecore/StringRef.h"
#include "treecore/Thread.h"
#include "treecore/WaitableEvent.h"

#include "treecore/native/win32_ChildProcess.h"
#include "treecore/native/win32_HighResolutionTimer.h"
#include "treecore/native/win32_InterProcessLock.h"

namespace treecore
{

HWND MESSAGE_WINDOW_HANDLE = 0;  // (this is used by other parts of the codebase)

void* getUser32Function( const char* functionName )
{
    HMODULE module = GetModuleHandleA( "user32.dll" );
    treecore_assert( module != 0 );
    return (void*) GetProcAddress( module, functionName );
}

//==============================================================================
CriticalSection::CriticalSection() noexcept
{
    // (just to check the MS haven't changed this structure and broken things...)
    static_assert( sizeof(CRITICAL_SECTION) <= sizeof(lock), "" );
    InitializeCriticalSection( (CRITICAL_SECTION*) lock );
}

CriticalSection::~CriticalSection() noexcept        { DeleteCriticalSection( (CRITICAL_SECTION*) lock ); }
void CriticalSection::enter() const noexcept        { EnterCriticalSection( (CRITICAL_SECTION*) lock ); }
bool CriticalSection::tryEnter() const noexcept     { return TryEnterCriticalSection( (CRITICAL_SECTION*) lock ) != FALSE; }
void CriticalSection::exit() const noexcept         { LeaveCriticalSection( (CRITICAL_SECTION*) lock ); }

//==============================================================================
WaitableEvent::WaitableEvent ( const bool manualReset ) noexcept
    : handle( CreateEvent( 0, manualReset ? TRUE : FALSE, FALSE, 0 ) ) {}

WaitableEvent::~WaitableEvent() noexcept        { CloseHandle( handle ); }

void WaitableEvent::signal() const noexcept     { SetEvent( handle ); }
void WaitableEvent::reset() const noexcept      { ResetEvent( handle ); }

bool WaitableEvent::wait( const int timeOutMs ) const noexcept
{
    return WaitForSingleObject( handle, (DWORD) timeOutMs ) == WAIT_OBJECT_0;
}

//==============================================================================
void TREECORE_SHARED_API _thread_entry_point( void* );

static unsigned int __stdcall threadEntryProc( void* userData )
{
    if (MESSAGE_WINDOW_HANDLE != 0)
        AttachThreadInput( GetWindowThreadProcessId( MESSAGE_WINDOW_HANDLE, 0 ),
                           GetCurrentThreadId(), TRUE );

    _thread_entry_point( userData ); // actually userData->ThreadEntryPoint()

    _endthreadex( 0 );
    return 0;
}

void Thread::launchThread()
{
    unsigned int newThreadId = 0;
    threadHandle = (void*) _beginthreadex( 0, 0, &threadEntryProc, this, 0, &newThreadId );
    if (threadHandle == 0)
        abort();
    threadId = (ThreadID) newThreadId;
}

void Thread::closeThreadHandle()
{
    CloseHandle( (HANDLE) threadHandle );
    threadId = 0;
    threadHandle = 0;
}

void Thread::killThread()
{
    if (threadHandle != 0)
    {
       #if TREECORE_DEBUG
        OutputDebugStringA( "** Warning - Forced thread termination **\n" );
       #endif
        TerminateThread( threadHandle, 0 );
    }
}

void TREECORE_STDCALL Thread::setCurrentThreadName( const String& name )
{
   #if TREECORE_DEBUG && TREECORE_COMPILER_MSVC
    struct
    {
        DWORD  dwType;
        LPCSTR szName;
        DWORD  dwThreadID;
        DWORD  dwFlags;
    } info;

    info.dwType = 0x1000;
    info.szName = name.toUTF8();
    info.dwThreadID = GetCurrentThreadId();
    info.dwFlags    = 0;

    __try
    {
        RaiseException( 0x406d1388 /*MS_VC_EXCEPTION*/, 0, sizeof(info) / sizeof(ULONG_PTR), (ULONG_PTR*) &info );
    }
    __except(EXCEPTION_CONTINUE_EXECUTION)
    {}
   #else
    (void) name;
   #endif
}

Thread::ThreadID TREECORE_STDCALL Thread::getCurrentThreadId()
{
    return (ThreadID) (pointer_sized_int) GetCurrentThreadId();
}

bool Thread::setThreadPriority( void* handle, int priority )
{
    int pri = THREAD_PRIORITY_TIME_CRITICAL;

    if (priority < 1)       pri = THREAD_PRIORITY_IDLE;
    else if (priority < 2)  pri = THREAD_PRIORITY_LOWEST;
    else if (priority < 5)  pri = THREAD_PRIORITY_BELOW_NORMAL;
    else if (priority < 7)  pri = THREAD_PRIORITY_NORMAL;
    else if (priority < 9)  pri = THREAD_PRIORITY_ABOVE_NORMAL;
    else if (priority < 10) pri = THREAD_PRIORITY_HIGHEST;

    if (handle == 0)
        handle = GetCurrentThread();

    return SetThreadPriority( handle, pri ) != FALSE;
}

void TREECORE_STDCALL Thread::setCurrentThreadAffinityMask( const uint32 affinityMask )
{
    SetThreadAffinityMask( GetCurrentThread(), affinityMask );
}

//==============================================================================
struct SleepEvent
{
    SleepEvent() noexcept
        : handle( CreateEvent( nullptr, FALSE, FALSE,
                              #if TREECORE_DEBUG
                               _T( "Sleep Event" ) ) )
         #else
                               nullptr) )
                              #endif
    {}

    ~SleepEvent() noexcept
    {
        CloseHandle( handle );
        handle = 0;
    }

    HANDLE handle;
};

static SleepEvent sleepEvent;

void TREECORE_STDCALL Thread::sleep( const int millisecs )
{
    treecore_assert( millisecs >= 0 );

    if (millisecs >= 10 || sleepEvent.handle == 0)
        Sleep( (DWORD) millisecs );
    else
        // unlike Sleep() this is guaranteed to return to the current thread after
        // the time expires, so we'll use this for short waits, which are more likely
        // to need to be accurate
        WaitForSingleObject( sleepEvent.handle, (DWORD) millisecs );
}

void Thread::yield()
{
    Sleep( 0 );
}

//==============================================================================
static int lastProcessPriority = -1;

// called when the app gains focus because Windows does weird things to process priority
// when you swap apps, and this forces an update when the app is brought to the front.
void TREECORE_STDCALL repeatLastProcessPriority()
{
    if (lastProcessPriority >= 0) // (avoid changing this if it's not been explicitly set by the app..)
    {
        DWORD p;

        switch (lastProcessPriority)
        {
        case Process::LowPriority:          p = IDLE_PRIORITY_CLASS; break;
        case Process::NormalPriority:       p = NORMAL_PRIORITY_CLASS; break;
        case Process::HighPriority:         p = HIGH_PRIORITY_CLASS; break;
        case Process::RealtimePriority:     p = REALTIME_PRIORITY_CLASS; break;
        default:                            treecore_assert_false; return;     // bad priority value
        }

        SetPriorityClass( GetCurrentProcess(), p );
    }
}

void TREECORE_STDCALL Process::setPriority( ProcessPriority prior )
{
    if (lastProcessPriority != (int) prior)
    {
        lastProcessPriority = (int) prior;
        repeatLastProcessPriority();
    }
}

bool TREECORE_STDCALL Process::isRunningUnderDebugger()
{
    return IsDebuggerPresent() != FALSE();
}

static void* currentModuleHandle = nullptr;

void* TREECORE_STDCALL Process::getCurrentModuleInstanceHandle() noexcept
{
    if (currentModuleHandle == nullptr)
        currentModuleHandle = GetModuleHandleA( nullptr );

    return currentModuleHandle;
}

void TREECORE_STDCALL Process::setCurrentModuleInstanceHandle( void* const newHandle ) noexcept
{
    currentModuleHandle = newHandle;
}

void TREECORE_STDCALL Process::raisePrivilege()
{
    treecore_assert_false; // xxx not implemented
}

void TREECORE_STDCALL Process::lowerPrivilege()
{
    treecore_assert_false; // xxx not implemented
}

void TREECORE_STDCALL Process::terminate()
{
   #if TREECORE_COMPILER_MSVC && TREECORE_CHECK_MEMORY_LEAKS
    _CrtDumpMemoryLeaks();
   #endif

    // bullet in the head in case there's a problem shutting down..
    ExitProcess( 1 );
}

int32 TREECORE_STDCALL Process::getProcessID()
{
    return GetCurrentProcessId();
}

bool TREECORE_STDCALL isRunningInWine()
{
    HMODULE ntdll = GetModuleHandleA( "ntdll" );
    return ntdll != 0 && GetProcAddress( ntdll, "wine_get_version" ) != nullptr;
}

//==============================================================================
bool DynamicLibrary::open( const String& name )
{
    close();
    handle = LoadLibrary( name.toWideCharPointer() );

    return handle != nullptr;
}

void DynamicLibrary::close()
{

    if (handle != nullptr)
    {
        FreeLibrary( (HMODULE) handle );
        handle = nullptr;
    }
}

void* DynamicLibrary::getFunction( const String& functionName ) noexcept
{
    return handle != nullptr ? (void*) GetProcAddress( (HMODULE) handle, functionName.toUTF8() ) // (void* cast is required for mingw)
           : nullptr;
}

//==============================================================================

InterProcessLock::InterProcessLock ( const String& name_ )
    : name( name_ )
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

InterProcessLock::Pimpl::Pimpl ( String name, const int timeOutMillisecs )
    : handle( 0 ), refCount( 1 )
{
    name   = name.replaceCharacter( '\\', '/' );
    handle = CreateMutexW( 0, TRUE, ("Global\\" + name).toWideCharPointer() );

    // Not 100% sure why a global mutex sometimes can't be allocated, but if it fails, fall back to
    // a local one. (A local one also sometimes fails on other machines so neither type appears to be
    // universally reliable)
    if (handle == 0)
        handle = CreateMutexW( 0, TRUE, ("Local\\" + name).toWideCharPointer() );

    if (handle != 0 && GetLastError() == ERROR_ALREADY_EXISTS)
    {
        if (timeOutMillisecs == 0)
        {
            close();
            return;
        }

        switch ( WaitForSingleObject( handle, timeOutMillisecs < 0 ? INFINITE : timeOutMillisecs ) )
        {
        case WAIT_OBJECT_0:
        case WAIT_ABANDONED:
            break;

        case WAIT_TIMEOUT:
        default:
            close();
            break;
        }
    }
}

InterProcessLock::Pimpl::~Pimpl()
{
    close();
}

void InterProcessLock::Pimpl::close()
{
    if (handle != 0)
    {
        ReleaseMutex( handle );
        CloseHandle( handle );
        handle = 0;
    }
}

HighResolutionTimer::Pimpl::Pimpl ( HighResolutionTimer& t ) noexcept: owner( t ), periodMs( 0 )
{}

HighResolutionTimer::Pimpl::~Pimpl()
{
    treecore_assert( periodMs == 0 );
}

void HighResolutionTimer::Pimpl::start( int newPeriod )
{
    if (newPeriod != periodMs)
    {
        stop();
        periodMs = newPeriod;

        TIMECAPS tc;
        if (timeGetDevCaps( &tc, sizeof(tc) ) == TIMERR_NOERROR)
        {
            const int actualPeriod = jlimit( (int) tc.wPeriodMin, (int) tc.wPeriodMax, newPeriod );

            timerID = timeSetEvent( actualPeriod, tc.wPeriodMin, callbackFunction, (DWORD_PTR) this,
                                    TIME_PERIODIC | TIME_CALLBACK_FUNCTION | 0x100 /*TIME_KILL_SYNCHRONOUS*/ );
        }
    }
}

void HighResolutionTimer::Pimpl::stop()
{
    periodMs = 0;
    timeKillEvent( timerID );
}

void __stdcall HighResolutionTimer::Pimpl::callbackFunction( UINT, UINT, DWORD_PTR userInfo, DWORD_PTR, DWORD_PTR )
{
    if ( Pimpl* const timer = reinterpret_cast<Pimpl*>(userInfo) )
        if (timer->periodMs != 0)
            timer->owner.hiResTimerCallback();
}

} // namespace treecore
