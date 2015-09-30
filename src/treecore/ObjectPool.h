#ifndef ZObjPofdsfdseeee344ol_h__
#define ZObjPofdsfdseeee344ol_h__

#include "treecore/AlignedMalloc.h"
#include "treecore/IntTypes.h"
#include "treecore/LFQueue.h"
#include "treecore/Queue.h"
#include "treecore/LeakedObjectDetector.h"

namespace treecore
{

template<typename T, bool MULTI_THREAD = true, int BLOCK_SIZE = 4*1024>
class ObjectPool
{
    class ObjBlock: public AlignedMalloc<AlignOf<T>::value>
    {
    public:
        ObjBlock();
        ~ObjBlock();
        RawType* operator[]( const int i );

    private:
        int8 m_elements[sizeof(T) * BLOCK_SIZE];
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ObjBlock);
    };

    typedef typename mpl_type_if<MULTI_THREAD, LfQueue<ObjBlock*> , Queue<ObjBlock*>>::type BlockQueueType;
    typedef typename mpl_type_if<MULTI_THREAD, LfQueue<T*>, Queue<T*>>::type ValueQueueType;

public:
    ObjectPool(int preAllocBlkNum=1);

    /**
     * After released, elements created from this pool became invalid
     */
    ~ObjectPool();

    T* Pop();

    template<typename... InitType>
    T* Pop(InitType&... initValues);

    void Push(T* k);

    //* 手动创建一些新元素,并加入池中
    void CreateSome(int allocBlkNum = 1);

private:
    BlockQueueType m_blocks;
    ValueQueueType m_reuseBuffer;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ObjectPool);
};

//==================================================================================================

template<typename T, bool MULTI_THREAD, int BLOCK_SIZE>
ObjectPool<T, MULTI_THREAD, BLOCK_SIZE>::ObjectPool(int preAllocBlkNum)
    : m_blocks(11) //2^11个,够了
    , m_reuseBuffer(17) //2^17个空位置,应该够了吧
{
    //static_assert( IS_TRIVIAL( T ) , "T must be pod!!!" );
    CreateSome(preAllocBlkNum);
}

template<typename T,bool MULTI_THREAD,int BLOCK_SIZE>
ObjectPool<T,MULTI_THREAD,BLOCK_SIZE>::~ObjectPool()
{
    ObjBlock* k;
    likely_while(m_blocks.pop(k)) { delete k; }
}

template<typename T,bool MULTI_THREAD,int BLOCK_SIZE>
template<typename... InitType>
T* ObjectPool<T,MULTI_THREAD,BLOCK_SIZE>::Pop(InitType&... initValues)
{
    RawType* k;
    unlikely_while(!m_reuseBuffer.pop(k)) {
        CreateSome();
    }
    T* x = new(k)T(initValues...);
    return x;
}

template<typename T,bool MULTI_THREAD,int BLOCK_SIZE>
T* ObjectPool<T,MULTI_THREAD,BLOCK_SIZE>::Pop()
{
    RawType* k;
    unlikely_while(!m_reuseBuffer.pop(k)) {
        CreateSome();
    }
    T* x= new(k) T();
    return x;
}


template<typename T,bool MULTI_THREAD,int BLOCK_SIZE>
void ObjectPool<T,MULTI_THREAD,BLOCK_SIZE>::Push(T* k)
{
    unlikely_if(k == nullptr) return;
    k->~T();
    m_reuseBuffer.push( (RawType*)(k) );
    // 这里应该没问题,为什么呢? 因为k必须是T类型的,而T类型肯定是从这个pool里面放出去的才对,push回来的不可能是个子类,
    // 即使是子类,也已经在push的时候转回T了
}


template<typename T,bool MULTI_THREAD,int BLOCK_SIZE>
void ObjectPool<T,MULTI_THREAD,BLOCK_SIZE>::CreateSome(int allocBlkNum)
{
    jassert( allocBlkNum>0 );
    int i=0;
    do {
        ObjBlock*const m = new ObjBlock;
        m_blocks.push(m);
        for(int i = 0; i < BLOCK_SIZE; ++i) {
            m_reuseBuffer.push( (*m)[i] );
        }
    } unlikely_while( ++i < allocBlkNum );
}

template<typename T,bool MULTI_THREAD,int BLOCK_SIZE>
typename ObjectPool<T , MULTI_THREAD , BLOCK_SIZE>::RawType* ObjectPool<T , MULTI_THREAD , BLOCK_SIZE>::ObjBlock::operator[]( const int i )
{
    return &m_elements[i];
}

template<typename T,bool MULTI_THREAD,int BLOCK_SIZE>
ObjectPool<T,MULTI_THREAD,BLOCK_SIZE>::ObjBlock::ObjBlock()
{}


template<typename T,bool MULTI_THREAD,int BLOCK_SIZE>
ObjectPool<T,MULTI_THREAD,BLOCK_SIZE>::ObjBlock::~ObjBlock()
{}

//============================================================================================

template<typename T , bool MULTI_THREAD = true , int allocNum = 4 * 1024>
class CreateFromPool : public SharedSingleton< ObjectPool<T , MULTI_THREAD , allocNum> >::EarlyUser
{
private:
    typedef SharedSingleton< ObjectPool<T , MULTI_THREAD , allocNum> > CreatePool;
public:
    static forcedinline void CreateSome(int blkNum=1)
    {
        CreatePool::getInstance().CreateSome(blkNum);
    }
    static forcedinline T* alloc()
    {
        return CreatePool::getInstance().Pop();
    };
    template<typename... InitType>
    static forcedinline T* alloc( InitType&... initTypes )
    {
        return CreatePool::getInstance().Pop( initTypes... );
    };
    static forcedinline void free( T* ptr )
    {
        return CreatePool::getInstance().Push(ptr);
    };
protected:
    TRIVIAL_CLASS(CreateFromPool);
};

} // namespace treecore

#endif
