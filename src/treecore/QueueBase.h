#ifndef TREECORE_QUEUE_BASE_H
#define TREECORE_QUEUE_BASE_H

#include "treecore/HeapBlock.h"
#include "treecore/LeakedObjectDetector.h"
#include "treecore/PlatformDefs.h"

namespace treecore
{
namespace impl
{

template<typename T,typename IntType>
class QueueBase
{
protected:
    explicit inline QueueBase(const IntType pow2size= 12 ,const bool setToZero = true) noexcept
        : m_p2size(pow2size)
        , m_sizeDec( ( 1u << pow2size ) - 1 )
        , m_data(m_sizeDec+1,setToZero)
    {
        treecore_assert( m_p2size >= 1 );
        checkPowerOfTwo(m_sizeDec + 1);
    }

    inline ~QueueBase(){}

    forcedinline IntType mod(IntType i) const noexcept{ return i&m_sizeDec; }

    forcedinline IntType getUsedSize(IntType writeStart,IntType readStart) const
    {
        const IntType temp2 = this->mod(writeStart) - this->mod(readStart);
        return this->mod(temp2);
    }

    forcedinline T& getDataInModIndex(IntType index) noexcept
    {
        checkPowerOfTwo(m_sizeDec + 1);
        return m_data[index&m_sizeDec];
    }

public:
    forcedinline T& operator[] (IntType index) const noexcept
    {
        checkPowerOfTwo(m_sizeDec + 1);
        return m_data[index&m_sizeDec];
    }

    forcedinline void clear(size_t numElements) noexcept{ m_data.clear(numElements); }

    /** 按照newsize重新分配内存,newsize>=oldsize.然后将旧的元素复制到新的size开辟的内存中
            ,注意,这时的start下标应该从0开始了.而end下标为返回值
        */
    forcedinline IntType realloc(const IntType newP2Size, IntType startPos, IntType writePos, bool const setToZero = true)
    {
        const IntType newSize = ( 1u << newP2Size );
        treecore_assert( newSize >= ( m_sizeDec + 1 ) );

        IntType const oldSize = getUsedSize(writePos,startPos);
        treecore_assert(oldSize <= m_sizeDec);

        HeapBlock<T> temp(newSize,setToZero);
        for(IntType i = 0; i < oldSize; ++i) {
            temp[i] = m_data[( i + startPos )&m_sizeDec];
        }
        m_data.swapWith(temp);
        m_sizeDec = newSize - 1;
        checkPowerOfTwo(m_sizeDec + 1);
        return oldSize;
    }

protected:
    IntType m_p2size;

private:
    IntType m_sizeDec;
    HeapBlock<T> m_data;
    TREECORE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(QueueBase);
}; // class QueueBase

} // namespace impl
} // namespace treecore

#endif // TREECORE_QUEUE_BASE_H
