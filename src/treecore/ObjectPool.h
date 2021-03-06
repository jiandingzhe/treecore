﻿#ifndef TREECORE_OBJECT_POOL_H
#define TREECORE_OBJECT_POOL_H

#include "treecore/AlignedMalloc.h"
#include "treecore/IntTypes.h"
#include "treecore/LeakedObjectDetector.h"
#include "treecore/LFQueue.h"
#include "treecore/MPL.h"
#include "treecore/Queue.h"
#include "treecore/RefCountObject.h"
#include "treecore/RefCountSingleton.h"

#include <type_traits>

class TestFramework;

namespace treecore
{

/**
 * @brief hold memory cache for fast creation of many objects
 *
 */
template<typename T, bool MULTI_THREAD = true, int BLOCK_SIZE = 4096>
class ObjectPool: public RefCountObject, public RefCountSingleton<ObjectPool<T, MULTI_THREAD, BLOCK_SIZE> >
{
    friend class ::TestFramework;

    struct ObjBlock
    {
        ObjBlock()
        {
        }

        ~ObjBlock() 
        {
        }

        T* operator[]( const int i )
        {
            T* p_data = (T*) &m_data;
            return p_data + i;
        }

        typename std::aligned_storage<sizeof(T) * BLOCK_SIZE, TREECORE_ALIGNOF(T)>::type m_data;

        TREECORE_DECLARE_NON_COPYABLE(ObjBlock)
    };

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
        while likely(m_blocks.pop(k)) { delete k; }
    }

    /**
     * @brief get one object which is built by its default constructor
     * @return object of type T
     */
    T* generate()
    {
        T* k = nullptr;
        while unlikely(!m_objects.pop(k))
        {
            createSome();
        }

        treecore_assert(k != nullptr);
        new (k) T();
        return k;
    }


    /**
     * @brief get one object with specified constructor parameters
     * @return object of type T
     */
    template<typename... InitType>
    T* generate(InitType&&... initValues)
    {
        T* k = nullptr;
        while unlikely(!m_objects.pop(k))
        {
            createSome();
        }

        treecore_assert(k != nullptr);
        new (k) T(initValues...);
        return k;
    }

    /**
     * @brief recycle this object
     * @param k object to recycle
     */
    void recycle(T* k)
    {
        if unlikely(k == nullptr) return;
        k->~T();
        m_objects.push(k);
    }

    /**
     * @brief create some additional memory blocks
     * @param numBlock number of memory blocks to create
     */
    void createSome(int numBlock = 1)
    {
        treecore_assert( numBlock>0 );

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
    TREECORE_DECLARE_NON_COPYABLE(ObjectPool)
};

} // namespace treecore

#endif // TREECORE_OBJECT_POOL_H
