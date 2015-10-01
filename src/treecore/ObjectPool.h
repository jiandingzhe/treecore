#ifndef ZObjPofdsfdseeee344ol_h__
#define ZObjPofdsfdseeee344ol_h__

#include "treecore/AlignedMalloc.h"
#include "treecore/IntTypes.h"
#include "treecore/LeakedObjectDetector.h"
#include "treecore/LFQueue.h"
#include "treecore/MPL.h"
#include "treecore/Queue.h"

class TestFramework;

namespace treecore
{

/**
 * @brief hold memory cache for fast creation of many objects
 *
 */
template<typename T, bool MULTI_THREAD = true, int BLOCK_SIZE = 4096>
class ObjectPool
{
    friend class ::TestFramework;

    TREECORE_ALN_BEGIN(alignof(T)) struct ObjBlock
    {
        ObjBlock() {}
        ~ObjBlock() {}

        T* operator[]( const int i )
        {
            return (T*) &m_data[i*sizeof(T)];
        }

        int8 m_data[sizeof(T) * BLOCK_SIZE];

        TREECORE_ALIGNED_ALLOCATOR(ObjBlock);
        TREECORE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ObjBlock);
    } TREECORE_ALN_END(alignof(T));

    typedef typename mpl_type_if<MULTI_THREAD, LfQueue<ObjBlock*> , Queue<ObjBlock*>>::type BlockQueueType;
    typedef typename mpl_type_if<MULTI_THREAD, LfQueue<T*>, Queue<T*>>::type ValueQueueType;

public:
    typedef T ValueType;

    /**
     * @brief create object pool
     * @param num_blocks_init number of initially built blocks
     */
    ObjectPool(int num_blocks_init = 1)
        : m_blocks(11) //2^11个,够了
        , m_objects(17) //2^17个空位置,应该够了吧
    {
        createSome(num_blocks_init);
    }

    /**
     * @brief After released, elements created from this pool became invalid
     */
    ~ObjectPool()
    {
        ObjBlock* k;
        likely_while(m_blocks.pop(k)) { delete k; }
    }

    /**
     * @brief get one object which is built by its default constructor
     * @return object of type T
     */
    T* generate()
    {
        T* k = nullptr;
        unlikely_if(!m_objects.pop(k))
        {
            createSome();
        }

        new (k) T();
        return k;
    }


    /**
     * @brief get one object with specified constructor parameters
     * @return object of type T
     */
    template<typename... InitType>
    T* generate(InitType... initValues) // TODO ref type or value type?
    {
        T* k = nullptr;
        unlikely_if(!m_objects.pop(k))
        {
            createSome();
        }

        new (k) T(initValues...);
        return k;
    }

    /**
     * @brief recycle this object
     * @param k object to recycle
     */
    void recycle(T* k)
    {
        unlikely_if(k == nullptr) return;
        k->~T();
        m_objects.push(k);
    }

    /**
     * @brief create some additional memory blocks
     * @param numBlock number of memory blocks to create
     */
    void createSome(int numBlock = 1)
    {
        jassert( numBlock>0 );


        for (int i = 0; i < numBlock; i++)
        {
            ObjBlock*const blk = new ObjBlock();
            m_blocks.push(blk);

            for (int j = 0; j < BLOCK_SIZE; ++j) {
                m_objects.push( (*blk)[j] );
            }
        }
    }

private:
    BlockQueueType m_blocks;
    ValueQueueType m_objects;
    TREECORE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ObjectPool);
};


//template<typename T , bool MULTI_THREAD = true , int allocNum = 4 * 1024>
//class CreateFromPool : public SharedSingleton< ObjectPool<T , MULTI_THREAD , allocNum> >::EarlyUser
//{
//private:
//    typedef SharedSingleton< ObjectPool<T , MULTI_THREAD , allocNum> > CreatePool;
//public:
//    static forcedinline void CreateSome(int blkNum=1)
//    {
//        CreatePool::getInstance().CreateSome(blkNum);
//    }
//    static forcedinline T* alloc()
//    {
//        return CreatePool::getInstance().Pop();
//    };
//    template<typename... InitType>
//    static forcedinline T* alloc( InitType&... initTypes )
//    {
//        return CreatePool::getInstance().Pop( initTypes... );
//    };
//    static forcedinline void free( T* ptr )
//    {
//        return CreatePool::getInstance().Push(ptr);
//    };
//protected:
//    TRIVIAL_CLASS(CreateFromPool);
//};

} // namespace treecore

#endif
