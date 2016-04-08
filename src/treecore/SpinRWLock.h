#ifndef TREECORE_SPIN_RW_LOCK_H
#define TREECORE_SPIN_RW_LOCK_H

#include "treecore/AtomicObject.h"
#include "treecore/ClassUtils.h"
#include "treecore/LeakedObjectDetector.h"
#include "treecore/PlatformDefs.h"

namespace treecore
{

/************************************************************************/
/* 读写锁,同一时间只能有1个writer,但可以同时有多个reader来读.
 */
/************************************************************************/
class SpinRWLock
{
public:
    class ScopedReadLock
    {
public:
        forcedinline ScopedReadLock( SpinRWLock& lock ) noexcept
            : m_lock( lock )
        {
            m_lock.EnterRead();
        }
        forcedinline ~ScopedReadLock() noexcept
        {
            m_lock.ExitRead();
        }
private:
        SpinRWLock& m_lock;
        TREECORE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR( ScopedReadLock );
    };
    class ScopedWriteLock
    {
public:
        forcedinline ScopedWriteLock( SpinRWLock& lock ) noexcept
            : m_lock( lock )
        {
            m_lock.EnterWrite();
        }
        forcedinline ~ScopedWriteLock() noexcept
        {
            m_lock.ExitWrite();
        }
private:
        SpinRWLock& m_lock;
        TREECORE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR( ScopedWriteLock );
    };
public:
    forcedinline SpinRWLock() noexcept: m_lockFlag( 0 ) {}
    forcedinline ~SpinRWLock() noexcept {};
    void EnterRead();
    forcedinline void ExitRead() noexcept
    {
        --m_lockFlag;
    }
    int EnterWriteAny();
    void EnterWrite() noexcept;
    /****************************************************
     * 当EnterWrite时,一定要注意,如果EnterWriteAny返回-1,
          说明已经有其他线程进入Write,此时我们不能调用ExitWrite因为当前线程并没有持有锁!
     ******************************************************/
    forcedinline void ExitWrite() noexcept
    {
#if TREECORE_DEBUG
        const bool temp2 = m_lockFlag.compare_set( -1, 0 );
        treecore_assert( temp2 ); //因为只有一个线程能修改m_reallocLock,所以此处必然成功
#else
        m_lockFlag = 0;
#endif
    }
private:
    AtomicObject<int> m_lockFlag;
    TREECORE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR( SpinRWLock );
};

} // namespace treecore

#endif // TREECORE_SPIN_RW_LOCK_H
