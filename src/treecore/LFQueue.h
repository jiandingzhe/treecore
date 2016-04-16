#ifndef ztd_ZlogfdssssssssssckfreeCgggggggggggggircularBuffer_h__ffffffffffffffff
#define ztd_ZlogfdssssssssssckfreeCgggggggggggggircularBuffer_h__ffffffffffffffff

#include "treecore/AtomicObject.h"
#include "treecore/ClassUtils.h"
#include "treecore/IntTypes.h"
#include "treecore/QueueBase.h"
#include "treecore/SpinRWLock.h"

namespace treecore
{

/*
   环形缓冲区需要3种状态:
   空: startPos==writePos
   满: startPos==(writePos+1) //因此,我们最多只能往缓冲区里放入cycSize-1个元素,最后一个元素是废的

   非环形缓冲区:size=writePos-startPos;
   环形缓冲区: size = (writePos<startPos)?(writePos+cycSize-startPos):(writePos-startPos);

   和环形缓冲区一样,我们使用两种策略,一种是静态的数组,另一种是非静态的数组.
   静态数组要在编译器确定尺寸.

   关于ABA: LfQueue使用连续的内存块存储节点,我们设定最大值是uint32的最大值,应该能够满足大多数情况了,
         无论在64位还是32位上,我们都使用双32位作为dub-CAS,这里还有些问题可能没想明白,似乎不需要dub-CAS就能搞定
                 这种结构的ABA问题,但一时懒得去想了.我们只是去应对极端情况.

 */

/*

    What we are talking about when we talking about Lockfree Queue ?

        (本文中全部index从1开始计数)

        1.当第一个push被卡住,后面的push怎么办?
            假设出于某种原因,我们第一个push被卡住了,那么当我们第一次进行pop的时候,
                由于第一个push还没有完成,pop便不能pop这个还没有push完的元素,那么问题出现了,
                它应该"跨过"这个还没写完的元素,去寻找后面可能已经写完的元素(这是有可能的)吗?
                这取决于,push操作是否允许我们跨过还未写完的元素去下一个元素进行push操作,这是允许的,
                那么就有可能发生,当第一个push未完成时,第二个push已经完成了的情况.这时我们有两种选择:

                1. 按照队列的正常逻辑,pop肯定是从队列的头部一个一个pop,那么如果第一个元素都没push完,
                   pop操作完全有理由相信整条队列都没有元素,连第一个都没写完,何谈第二个?

                2. 对于我们实现的lockfree队列来说,确实可能发生第一个push还未写完时,第二个push已经写完的情况,
                   否则如果第二个push操作需要等待第一个push操作,就不叫lockfree了.那么,按照这个逻辑,
                   当pop操作发现第一个push操作并未结束时,我们可以允许它"跨过"第一个push中的元素,去检查
                   第二个元素是否已经push成功并将其进行pop.这样有最大的效率.
                   这里有个问题,第一个元素是队列中的第一个元素,pop操作如果跨过了第一个元素,并pop成功了第二个元素
                   ,那对于pop操作来说,岂不是看上去第二个元素反倒变成了第一个元素??? ---- 实际上这个也是合理的,
                   如果第一个元素最先开始push,却慢于第二个元素push成功,那么从逻辑上说这个"第一个元素"实际上
                   本应该是第二个元素,因为他晚于第二个元素push成功.

                   但,如果我们真的允许pop操作跨过push中的元素去pop后面的元素,有一个巨大的问题是,这样做会
                   使队列变得"千疮百孔",队列中会留下大量的被跳过去的"孔洞",每个孔洞都是一个暂时未执行完的push操作,
                   只有在队列执行到末尾,再绕回来的时候他们才有机会被执行,并且这样会破坏这太可怕了. 如果真的这样做,
                   pop操作必须一旦发现队列是空的,就遍历整个队列逐个查找是否有遗漏的孔洞,如果有则pop,如果没有则返回空

                第二种情况能得到极高的效率,但pop线程比较占资源(要不断扫描),因此这个功能我们留到以后再去实现.
                (突然想到的,可以给没有push成功的,弄一个原子计数器,push成功了++,pop了就--,当队列指针startPos==writePos时,
                pop操作去查看计数器是否大于零,如果大于零,就必须遍历队列查找孔洞并pop).

 */

namespace impl
{

template<typename T>
struct LfQueueNode
{
public:
    TREECORE_TRIVIAL_CLASS( LfQueueNode );
public:
    T m_dataInNode;
    AtomicObject<int32> m_stateInNode;
};

class index_t
{
public:
    TREECORE_TRIVIAL_CLASS( index_t );
    forcedinline index_t( uint32 index_, uint32 count_ ): index( index_ ), count( count_ ) {}
    forcedinline operator uint32 () const { return index; }
public:
    uint32 index;
    uint32 count;
};

} // namespace impl

template<typename T>
class LfQueue: private impl::QueueBase<impl::LfQueueNode<T>, uint32>
{
public:
    typedef int mark_t;
public:
    using impl::QueueBase<impl::LfQueueNode<T>, uint32>::m_p2size;

    explicit forcedinline LfQueue( uint32 p2size = 12 )
        : impl::QueueBase<impl::LfQueueNode<T>, uint32>( p2size, true )
        , m_readPos( impl::index_t( 0, 0 ) )
        , m_writePos( impl::index_t( 0, 0 ) )
        , m_reallocLock()
    {
        //在push发生时,我们必须初始化m_data中所有的m_stateInNode为"FreeToUse",只有这样才能
        //保证当push发生时能够正确检测其接下来要读取的节点是FreeToUse的.
        //TODO!
        //static_assert( IS_TRIVIAL( impl::LfQueueNode<T> ) , "T must be pod!!!" );
    }

    forcedinline ~LfQueue() {}

    template<typename Func>
    forcedinline bool bound_push( const Func& func )
    {
#if ZTD_LFQUEUE_TEST_LOCK
        SpinLock::ScopedLockType test_lock( m_test_lock );
#endif

        using namespace impl;
        const SpinRWLock::ScopedReadLock scopedLock( m_reallocLock );

        for (;; )
        {
            const index_t readPos = m_readPos;
            const index_t k = m_writePos;
            if unlikely( _isFull( k.index, readPos.index ) ) return false;  //如果已经满了就返回

            mark_t mark = FreeToUse;
            this->getDataInModIndex( k ).m_stateInNode.compare_exchange( &mark, Pushing );
            if likely( mark == FreeToUse )
            {
                func( this->getDataInModIndex( k ).m_dataInNode );
                this->getDataInModIndex( k ).m_stateInNode.compare_set( Pushing, HasNode );
                m_writePos.compare_set( k, index_t( k + 1, k.count + 3 ) );
                break;
            }
            else if unlikely( mark == Poping )
            {
                return false; //如果等于Poping,我们可以想点别的办法,这里没想清楚,其实加锁也无妨,不是常见情况
            }
            else
            {
                treecore_assert( mark == Pushing || mark == HasNode );
                m_writePos.compare_set( k, index_t( k + 1, k.count + 3 ) );
                //有可能,例如两条线程A和B同时push,由于A的push行为如此之频繁,
                //使得B每次循环重试时总是获取不到正确的writePos,
                //极端情况下会导致这里重试多次(超过2000次),
                //但,总会找到空隙重试成功.线程之间不会发生等待或锁定
            }
        }
        return true;
    }

    template<typename Func>
    forcedinline bool pop( const Func& func )
    {
#if ZTD_LFQUEUE_TEST_LOCK
        SpinLock::ScopedLockType test_lock( m_test_lock );
#endif

        using namespace impl;
        const SpinRWLock::ScopedReadLock scopedLock( m_reallocLock );
        for (;; )
        {
            const index_t k = m_readPos;
            if unlikely( _isEmpty( m_writePos.load(), k ) )
                return false;

            mark_t mark = HasNode;
            this->getDataInModIndex( k ).m_stateInNode.compare_exchange( &mark, Poping );
            if likely( mark == HasNode )
            {
                T& a = this->getDataInModIndex( k ).m_dataInNode;
                func( a );
                this->getDataInModIndex( k ).m_stateInNode.compare_set( Poping, FreeToUse );
                m_readPos.compare_set( k, index_t( k + 1, k.count + 3 ) );
                break;
            }
            else if unlikely( mark == Pushing )
                {
                    //如果向队列中写数据的方法相当相当慢,这里便可能出现pushing并没结束时有人已经在pop的情况,这时,我们返回false,代表队列中没有元素(因为第一个要读的元素都没写完,那后面的即使写完了也访问不到).
                    //这不是锁,而是队列其确确实实没有东西
                    return false;
                }
                else
                {
                    treecore_assert( mark == FreeToUse || mark == Poping );
                    m_readPos.compare_set( k, index_t( k + 1, k.count + 3 ) );
                }
        }
        return true;
    }

    forcedinline bool pop( T& obj )
    {
        return pop( [&]( const T& k ) { treecore_assert( &k != nullptr ); obj = k; } );
    }

    template<typename Func>
    void lostable_push( const Func& func )
    {
        while unlikely( !bound_push( func ) ) {
            pop( []( const T& ) {} );
        }
    }

    template<typename Func>
    void push( const Func& func )
    {
        while unlikely( !bound_push( func ) ) {
            _Realloc();
        }
    }

    forcedinline bool bound_push( const T& obj )
    {
        return bound_push( [&]( T& k ) { k = obj; } );
    }

    forcedinline void lostable_push( const T& obj )
    {
        return lostable_push( [&]( T& k ) { k = obj; } );
    }

    forcedinline void push( const T& obj )
    {
        return push( [&]( T& k ) { k = obj; } );
    }

    forcedinline bool isFull()
    {
        const SpinRWLock::ScopedReadLock scopedLock( m_reallocLock );
        return _isFull( m_writePos.load(), m_readPos.load() );
    }

    forcedinline bool isEmpty()
    {
        const SpinRWLock::ScopedReadLock scopedLock( m_reallocLock );
        return _isEmpty( m_writePos.load(), m_readPos.load() );
    }

private:

    enum {
        FreeToUse = 0,  //此处应为0,因为初始化Node数组和_Realloc()时初始时Node的状态必须设定为freeToUse
        Pushing,
        HasNode,
        Poping
    };

    AtomicObject<impl::index_t> m_readPos;
    AtomicObject<impl::index_t> m_writePos;
    SpinRWLock m_reallocLock;

    forcedinline bool _isFull( uint32 writePos, uint32 startPos ) const noexcept
    {
        const uint32 k = this->mod( writePos + 1 );
        startPos = this->mod( startPos );
        return startPos == k;
    }

    forcedinline bool _isEmpty( uint32 writePos, uint32 startPos ) noexcept
    {
        writePos = this->mod( writePos );
        startPos = this->mod( startPos );
        return startPos == writePos;
    }

    void _Realloc()
    {
#if ZTD_LFQUEUE_TEST_LOCK
        SpinLock::ScopedLockType test_lock( m_test_lock );
#endif

        if (m_reallocLock.EnterWriteAny() == -1) return;
        uint32 const m = this->realloc( ++m_p2size, m_readPos.load(), m_writePos.load(), true );
        m_readPos.get_raw()->index = 0;
        m_writePos.get_raw()->index = m;
        m_reallocLock.ExitWrite();
    }

#if ZTD_LFQUEUE_TEST_LOCK
    SpinLock m_test_lock;
#endif

    TREECORE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR( LfQueue );
};

}

#endif // ztd_ZLfQueue_h__
