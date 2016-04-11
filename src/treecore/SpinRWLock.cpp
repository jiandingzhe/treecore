#include "treecore/SpinRWLock.h"

#include "treecore/PlatformDefs.h"
#include "treecore/Thread.h"

namespace treecore
{

void SpinRWLock::EnterRead()
{
    int k;
    for( int i = 0;; ++i ) {
        if unlikely( i == 40 ) { i = 0; Thread::sleep( 100 ); }
        k = m_lockFlag;
        treecore_assert( k >= -1 );
        if unlikely( k == -1 ) continue;
        if likely(m_lockFlag.compare_set(k, k+1)) break;
    }
}

int SpinRWLock::EnterWriteAny()
{
    int k;
    for( int i = 0;; ++i ) {
        if unlikely( i == 40 ) { i = 0; Thread::sleep( 100 ); }
        k = m_lockFlag.compare_set(0, -1);
        if likely( ( k==-1 )||( k==0 ) ) break;
    }
    return k;
}

void SpinRWLock::EnterWrite() noexcept
{
    int k;
    for( int i = 0;; ++i ) {
        if unlikely( i == 40 ) { i = 0; Thread::sleep( 100 ); }
        k = m_lockFlag.compare_set(0, -1);
        if likely( k==0 ) break;
    }
}

} // namespace treecore
