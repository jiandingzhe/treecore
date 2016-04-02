#include "treecore/SpinRWLock.h"

#include "treecore/PlatformDefs.h"
#include "treecore/Thread.h"

namespace treecore
{

void SpinRWLock::EnterRead()
{
    int k;
    for( int i = 0;; ++i ) {
        unlikely_if( i == 40 ) { i = 0; Thread::sleep( 100 ); }
        k = m_lockFlag;
        jassert( k >= -1 );
        unlikely_if( k == -1 ) continue;
        likely_if(m_lockFlag.compare_set(k, k+1)) break;
    }
}

int SpinRWLock::EnterWriteAny()
{
    int k;
    for( int i = 0;; ++i ) {
        unlikely_if( i == 40 ) { i = 0; Thread::sleep( 100 ); }
        k = m_lockFlag.compare_set(0, -1);
        likely_if( ( k==-1 )||( k==0 ) ) break;
    }
    return k;
}

void SpinRWLock::EnterWrite() noexcept
{
    int k;
    for( int i = 0;; ++i ) {
        unlikely_if( i == 40 ) { i = 0; Thread::sleep( 100 ); }
        k = m_lockFlag.compare_set(0, -1);
        likely_if( k==0 ) break;
    }
}

} // namespace treecore
