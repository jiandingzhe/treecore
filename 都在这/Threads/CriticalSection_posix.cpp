#ifndef JUCE_CRITICALSECTION_H_INCLUDED
#define JUCE_CRITICALSECTION_H_INCLUDED

#include "treecore/Config.h"
#include "treecore/MathsFunctions.h"
#include "treecore/StandardHeader.h"
#include "treecore/ScopedLock.h"

namespace treecore {

//==============================================================================
/**
    A re-entrant mutex.

    A CriticalSection acts as a re-entrant mutex object. The best way to lock and unlock
    one of these is by using RAII in the form of a local ScopedLock object - have a look
    through the codebase for many examples of how to do this.

    In almost all cases you'll want to declare your CriticalSection as a member variable.
    Occasionally you may want to declare one as a static variable, but in that case the usual
    C++ static object order-of-construction warnings should be heeded.

    @see ScopedLock, ScopedTryLock, ScopedUnlock, SpinLock, ReadWriteLock, Thread, InterProcessLock
*/
class JUCE_API  CriticalSection
{
public:
    
    /// @brief  创建一个临界区
    inline CriticalSection() noexcept
    {
        pthread_mutexattr_t atts;
        pthread_mutexattr_init( &atts );
        pthread_mutexattr_settype( &atts , PTHREAD_MUTEX_RECURSIVE );
        // 此协议值（如 thrd1）会影响线程的优先级和调度。如果更高优先级的线程因 thrd1 所拥有的一个或多个互斥锁而被阻塞，
        // 而这些互斥锁是用 PTHREAD_PRIO_INHERIT 初始化的，则 thrd1 将以高于它的优先级或者所有正在等待这些互斥锁
        // （这些互斥锁是 thrd1 指所拥有的互斥锁）的线程的最高优先级运行。
        TREECORE_DECLARE_IF( !TREECORE_TARGET_ANDROID , pthread_mutexattr_setprotocol( &atts , PTHREAD_PRIO_INHERIT ) );
        pthread_mutex_init( &lock , &atts );
        pthread_mutexattr_destroy( &atts );
    }

    /** Destructor.
        If the critical section is deleted whilst locked, any subsequent behaviour
        is unpredictable.
    */
    inline ~CriticalSection() noexcept
    {
        // 如果触发此断言,说明你试图在这个锁还在被锁住的时候
        // 就打算销毁它.这是一个错误行为.
        TREECORE_DEBUG_CODE( tassert( mHolders.get() == 0 ) );
        pthread_mutex_destroy( &lock );
    }

    /// @brief  持有锁.
    ///         此函数是可重入的.若当前无法持有锁,此函数
    ///         会一直等待直到能够持有锁为止.
    inline void enter() const noexcept
    {
        TREECORE_DEBUG_CODE( ++mHolders );
        pthread_mutex_lock( &lock ); 
    }

    /// @brief  尝试持有锁.
    ///         此函数是可重入的.
    /// @return 如果持有了锁则返回true,否则返回false
    inline bool tryEnter() const noexcept
    {
        const bool k = pthread_mutex_trylock( &lock ) == 0;
        TREECORE_DEBUG_CODE( if( k ) ++mHolders );
        return k;
    }

    /// @brief 释放锁
    ///        如果有当前锁被锁住了多次(介于这个类是可重入的,这是有可能的),那么
    ///        这个锁必须也被释放多次.
    inline void exit() const noexcept
    {
        TREECORE_DEBUG_CODE( --mHolders );
        pthread_mutex_unlock( &lock ); 
    }

private:
    mutable pthread_mutex_t lock;
    TREECORE_DEBUG_CODE( Atomic<int> mHolders );
    TREECORE_DECLARE_NON_COPYABLE(CriticalSection);
};


const CriticalSection::ScopedLock makeScopedLock( CriticalSection& lock )
{
    return CriticalSection::ScopedLock( lock );
}

#define TREECORE_SCOPED_LOCK(lockName) \
    const auto autoLock##__COUNTER__ = makeScopedLock(lockName)


#endif   // JUCE_CRITICALSECTION_H_INCLUDED
