#include "treecore/Thread.h"

#include <pthread.h>
#include <sys/time.h>

namespace treecore
{

CriticalSection::CriticalSection() noexcept
{
    pthread_mutexattr_t atts;
    pthread_mutexattr_init( &atts );
    pthread_mutexattr_settype( &atts, PTHREAD_MUTEX_RECURSIVE );
#if !TREECORE_OS_ANDROID
    pthread_mutexattr_setprotocol( &atts, PTHREAD_PRIO_INHERIT );
#endif
    pthread_mutex_init( &lock, &atts );
    pthread_mutexattr_destroy( &atts );
}

CriticalSection::~CriticalSection() noexcept
{
    pthread_mutex_destroy( &lock );
}

void CriticalSection::enter() const noexcept
{
    pthread_mutex_lock( &lock );
}

bool CriticalSection::tryEnter() const noexcept
{
    return pthread_mutex_trylock( &lock ) == 0;
}

void CriticalSection::exit() const noexcept
{
    pthread_mutex_unlock( &lock );
}

//
// Thread
//

void TREECORE_SHARED_API _thread_entry_point_( void* );

extern "C" void* threadEntryProc( void* );
extern "C" void* threadEntryProc( void* userData )
{
    TREECORE_AUTO_RELEASE_POOL
    {
#if TREECORE_OS_ANDROID
        const AndroidThreadScope androidEnv;
#endif

        _thread_entry_point_( userData );
    }

    return nullptr;
}

void Thread::launchThread()
{
    threadHandle = 0;
    pthread_t handle = 0;

    if (pthread_create( &handle, 0, threadEntryProc, this ) == 0)
    {
        pthread_detach( handle );
        threadHandle = (void*) handle;
        threadId = (ThreadID) threadHandle;
    }
}

void Thread::closeThreadHandle()
{
    threadId = 0;
    threadHandle = 0;
}

void Thread::killThread()
{
    if (threadHandle != 0)
    {
#if TREECORE_OS_ANDROID
        treecore_assert_false; // pthread_cancel not available!
#else
        pthread_cancel( pthread_t( threadHandle ) );
#endif
    }
}

void TREECORE_STDCALL Thread::setCurrentThreadName( const String& name )
{
#if TREECORE_OS_IOS || (TREECORE_OS_OSX && defined (MAC_OS_X_VERSION_10_5) && MAC_OS_X_VERSION_MIN_REQUIRED >= MAC_OS_X_VERSION_10_5)
    TREECORE_AUTO_RELEASE_POOL
    {
        [[NSThread currentThread] setName : juceStringToNS( name )];
    }
#elif TREECORE_OS_LINUX
#    if (__GLIBC__ * 1000 + __GLIBC_MINOR__) >= 2012
    pthread_setname_np( pthread_self(), name.toRawUTF8() );
#    else
    prctl( PR_SET_NAME, name.toRawUTF8(), 0, 0, 0 );
#    endif
#endif
}

bool Thread::setThreadPriority( void* handle, int priority )
{
    struct sched_param param;
    int policy;
    priority = jlimit( 0, 10, priority );

    if (handle == nullptr)
        handle = (void*) pthread_self();

    if (pthread_getschedparam( (pthread_t) handle, &policy, &param ) != 0)
        return false;

    policy = priority == 0 ? SCHED_OTHER : SCHED_RR;

    const int minPriority = sched_get_priority_min( policy );
    const int maxPriority = sched_get_priority_max( policy );

    param.sched_priority = ( (maxPriority - minPriority) * priority ) / 10 + minPriority;
    return pthread_setschedparam( pthread_t( handle ), policy, &param ) == 0;
}

Thread::ThreadID TREECORE_STDCALL Thread::getCurrentThreadId()
{
    return ThreadID( pthread_self() );
}

void TREECORE_STDCALL Thread::yield()
{
    sched_yield();
}

void TREECORE_STDCALL Thread::sleep( int millisecs )
{
    struct timespec time;
    time.tv_sec  = millisecs / 1000;
    time.tv_nsec = (millisecs % 1000) * 1000000;
    nanosleep( &time, nullptr );
}

#if defined (CPU_ISSET) && !defined (SUPPORT_AFFINITIES)
#    define SUPPORT_AFFINITIES 1
#endif

void TREECORE_STDCALL Thread::setCurrentThreadAffinityMask( const uint32 affinityMask )
{
#if SUPPORT_AFFINITIES
    cpu_set_t affinity;
    CPU_ZERO( &affinity );

    for (int i = 0; i < 32; ++i)
        if ( ( affinityMask & (1 << i) ) != 0 )
            CPU_SET( i, &affinity );

    /*
       N.B. If this line causes a compile error, then you've probably not got the latest
       version of glibc installed.

       If you don't want to update your copy of glibc and don't care about cpu affinities,
       then you can just disable all this stuff by setting the SUPPORT_AFFINITIES macro to 0.
     */
    sched_setaffinity( getpid(), sizeof(cpu_set_t), &affinity );
    sched_yield();

#else
    /* affinities aren't supported because either the appropriate header files weren't found,
       or the SUPPORT_AFFINITIES macro was turned off
     */
    treecore_assert_false;
    (void) affinityMask;
#endif
}

//
// WaitableEvent
//
WaitableEvent::WaitableEvent ( const bool useManualReset ) noexcept
    : triggered( false ), manualReset( useManualReset )
{
    pthread_cond_init( &condition, 0 );

    pthread_mutexattr_t atts;
    pthread_mutexattr_init( &atts );
#if !TREECORE_OS_ANDROID
    pthread_mutexattr_setprotocol( &atts, PTHREAD_PRIO_INHERIT );
#endif
    pthread_mutex_init( &mutex, &atts );
}

WaitableEvent::~WaitableEvent() noexcept
{
    pthread_cond_destroy( &condition );
    pthread_mutex_destroy( &mutex );
}

bool WaitableEvent::wait( const int timeOutMillisecs ) const noexcept
{
    pthread_mutex_lock( &mutex );

    if (!triggered)
    {
        if (timeOutMillisecs < 0)
        {
            do
            {
                pthread_cond_wait( &condition, &mutex );
            }
            while (!triggered);
        }
        else
        {
            struct timeval now;
            gettimeofday( &now, 0 );

            struct timespec time;
            time.tv_sec  = now.tv_sec  + (timeOutMillisecs / 1000);
            time.tv_nsec = ( now.tv_usec + ( (timeOutMillisecs % 1000) * 1000 ) ) * 1000;

            if (time.tv_nsec >= 1000000000)
            {
                time.tv_nsec -= 1000000000;
                time.tv_sec++;
            }

            do
            {
                if (pthread_cond_timedwait( &condition, &mutex, &time ) == ETIMEDOUT)
                {
                    pthread_mutex_unlock( &mutex );
                    return false;
                }
            }
            while (!triggered);
        }
    }

    if (!manualReset)
        triggered = false;

    pthread_mutex_unlock( &mutex );
    return true;
}

void WaitableEvent::signal() const noexcept
{
    pthread_mutex_lock( &mutex );

    if (!triggered)
    {
        triggered = true;
        pthread_cond_broadcast( &condition );
    }

    pthread_mutex_unlock( &mutex );
}

void WaitableEvent::reset() const noexcept
{
    pthread_mutex_lock( &mutex );
    triggered = false;
    pthread_mutex_unlock( &mutex );
}

} // namespace treecore
