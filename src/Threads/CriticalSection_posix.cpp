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
    
    /// @brief  ����һ���ٽ���
    inline CriticalSection() noexcept
    {
        pthread_mutexattr_t atts;
        pthread_mutexattr_init( &atts );
        pthread_mutexattr_settype( &atts , PTHREAD_MUTEX_RECURSIVE );
        // ��Э��ֵ���� thrd1����Ӱ���̵߳����ȼ��͵��ȡ�����������ȼ����߳��� thrd1 ��ӵ�е�һ����������������������
        // ����Щ���������� PTHREAD_PRIO_INHERIT ��ʼ���ģ��� thrd1 ���Ը����������ȼ������������ڵȴ���Щ������
        // ����Щ�������� thrd1 ָ��ӵ�еĻ����������̵߳�������ȼ����С�
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
        // ��������˶���,˵������ͼ����������ڱ���ס��ʱ��
        // �ʹ���������.����һ��������Ϊ.
        TREECORE_DEBUG_CODE( tassert( mHolders.get() == 0 ) );
        pthread_mutex_destroy( &lock );
    }

    /// @brief  ������.
    ///         �˺����ǿ������.����ǰ�޷�������,�˺���
    ///         ��һֱ�ȴ�ֱ���ܹ�������Ϊֹ.
    inline void enter() const noexcept
    {
        TREECORE_DEBUG_CODE( ++mHolders );
        pthread_mutex_lock( &lock ); 
    }

    /// @brief  ���Գ�����.
    ///         �˺����ǿ������.
    /// @return ������������򷵻�true,���򷵻�false
    inline bool tryEnter() const noexcept
    {
        const bool k = pthread_mutex_trylock( &lock ) == 0;
        TREECORE_DEBUG_CODE( if( k ) ++mHolders );
        return k;
    }

    /// @brief �ͷ���
    ///        ����е�ǰ������ס�˶��(����������ǿ������,�����п��ܵ�),��ô
    ///        ���������Ҳ���ͷŶ��.
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
