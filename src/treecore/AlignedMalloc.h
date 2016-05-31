#ifndef ztd_aligned_malloc_h__
#define ztd_aligned_malloc_h__

#include "treecore/Align.h"
#include "treecore/IntTypes.h"

#include <new>
#include <malloc.h>

namespace treecore
{

//* 分配对齐的内存
void* aligned_malloc(size_t length_byte_0, size_t const algn_val_ispw2);

//* 分配对齐的内存,并将值清零
void* aligned_calloc(size_t length_byte_0,size_t const algn_val_ispw2);

//* 释放对齐的内存,必须和aligned_malloc或者aligned_calloc一起使用,不能用来释放malloc的内存
void aligned_free(void*const ptr_a) noexcept;

//* 分配对齐的内存,并将值清零
void* aligned_realloc(void* ptr,size_t length_byte_0,size_t const algn_val_ispw2);

//--------------- malloc controlled by template -----------------------------------------------------------

namespace helper
{

template<size_t aln_sz, bool do_align>
struct AlignedMallocHelper {};

template<size_t aln_sz>
struct AlignedMallocHelper<aln_sz, true>
{
    static forcedinline void* malloc(size_t len) { return aligned_malloc(len, aln_sz); }

    static forcedinline void* calloc(size_t len) { return aligned_calloc(len, aln_sz); }

    static forcedinline void free(void* ptr) { aligned_free(ptr); }

    static forcedinline void* realloc(void* const ptr, size_t len) { return aligned_realloc(ptr, len, aln_sz); }
};

template<size_t aln_sz>
struct AlignedMallocHelper<aln_sz, false>
{
    static forcedinline void* malloc(size_t len) { return ::malloc(len); }

    static forcedinline void* calloc(size_t len) { return ::calloc(1, len); }

    static forcedinline void free(void* ptr) { ::free(ptr); }

    static forcedinline void* realloc(void* const ptr,size_t len) { return ::realloc(ptr, len); }
};

} // namespace helper

template<size_t alignSize>
forcedinline void* aligned_malloc(size_t len)
{
    return helper::AlignedMallocHelper<alignSize, 8<alignSize >::malloc(len);
}

template<size_t alignSize>
forcedinline void* aligned_calloc(size_t len)
{
    return helper::AlignedMallocHelper<alignSize, 8<alignSize >::calloc(len);
}

template<size_t alignSize>
forcedinline void aligned_free(void* ptr)
{
    helper::AlignedMallocHelper<alignSize, 8<alignSize >::free(ptr);
}

template<size_t alignSize>
forcedinline void* aligned_realloc(void* const ptr,size_t len)
{
    return helper::AlignedMallocHelper<alignSize, 8<alignSize >::realloc(ptr,len);
}


} // namespace treecore

#define TREECORE_ALIGNED_ALLOCATOR(_TYPE_) \
    void* operator new (std::size_t size)\
    {\
        void* ptr = treecore::aligned_malloc<TREECORE_ALIGNOF(_TYPE_)>(size);\
        if (!ptr)\
            throw std::bad_alloc();\
        return ptr;\
    }\
    \
    void* operator new (std::size_t size, const std::nothrow_t&) noexcept\
    {\
        return treecore::aligned_malloc<TREECORE_ALIGNOF(_TYPE_)>(size);\
    }\
    \
    void* operator new (std::size_t size, void* ptr) noexcept\
    {\
        treecore_assert(treecore::pointer_sized_uint(ptr) % TREECORE_ALIGNOF(_TYPE_) == 0);\
        return ptr;\
    }\
    void* operator new[] (std::size_t size)\
    {\
        void* ptr = treecore::aligned_malloc<TREECORE_ALIGNOF(_TYPE_)>(size);\
        if (!ptr)\
            throw std::bad_alloc();\
        return ptr;\
    }\
    \
    void* operator new[] (std::size_t size, const std::nothrow_t& nothrow) noexcept\
    {\
        return treecore::aligned_malloc<TREECORE_ALIGNOF(_TYPE_)>(size);\
    }\
    \
    void* operator new[] (std::size_t size, void* ptr) noexcept\
    {\
        treecore_assert(treecore::pointer_sized_uint(ptr) % TREECORE_ALIGNOF(_TYPE_) == 0);\
        return ptr;\
    }\
    \
    void operator delete (void* ptr) noexcept\
    {\
        treecore::aligned_free<TREECORE_ALIGNOF(_TYPE_)>(ptr);\
    }\
    \
    void operator delete (void* ptr, const std::nothrow_t& nothrow_constant) noexcept\
    {\
        treecore::aligned_free<TREECORE_ALIGNOF(_TYPE_)>(ptr);\
    }\
    \
    void operator delete (void* ptr, void* ptr2) noexcept\
    {\
        treecore_assert(treecore::pointer_sized_uint(ptr) % TREECORE_ALIGNOF(_TYPE_) == 0);\
    }\
    \
    void operator delete[] (void* ptr) noexcept\
    {\
        treecore::aligned_free<TREECORE_ALIGNOF(_TYPE_)>(ptr);\
    }\
    \
    void operator delete[] (void* ptr, const std::nothrow_t& nothrow_constant) noexcept\
    {\
        treecore::aligned_free<TREECORE_ALIGNOF(_TYPE_)>(ptr);\
    }\
    \
    void operator delete[] (void* ptr, void* voidptr2) noexcept\
    {\
        treecore_assert(treecore::pointer_sized_uint(ptr) % TREECORE_ALIGNOF(_TYPE_) == 0);\
    }

#endif // ztd_aligned_malloc_h__
