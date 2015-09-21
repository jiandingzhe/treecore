#ifndef ztd_aligned_malloc_h__
#define ztd_aligned_malloc_h__

#include "treecore/PlatformDefs.h"
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

//--------------- 以下是模板控制的malloc-----------------------------------------------------------

namespace helper
{

template<size_t alignSize>
class AlignedMallocHelper
{
public:
    static forcedinline void* malloc(size_t len) { return aligned_malloc(len,alignSize); }

    static forcedinline void* calloc(size_t len) { return aligned_calloc(len,alignSize); }

    static forcedinline void free(void* ptr) { aligned_free(ptr); }

    static forcedinline void* realloc(void* const ptr,size_t len) { return aligned_realloc(ptr,len,alignSize); }
};

template<>
class AlignedMallocHelper<0>
{
public:
    static forcedinline void* malloc(size_t len) { return ::malloc(len); }

    static forcedinline void* calloc(size_t len) { return ::calloc(1, len); }

    static forcedinline void free(void* ptr) { ::free(ptr); }

    static forcedinline void* realloc(void* const ptr,size_t len) { return ::realloc(ptr,len); }
};

} // namespace helper

template<size_t alignSize>
forcedinline void* aligned_malloc(size_t len) { return helper::AlignedMallocHelper<alignSize>::malloc(len); }

template<size_t alignSize>
forcedinline void* aligned_calloc(size_t len) { return helper::AlignedMallocHelper<alignSize>::calloc(len); }

template<size_t alignSize>
forcedinline void aligned_free(void* ptr) { helper::AlignedMallocHelper<alignSize>::free(ptr); }

template<size_t alignSize>
forcedinline void* aligned_realloc(void* const ptr,size_t len) { return helper::AlignedMallocHelper<alignSize>::realloc(ptr,len); }

template<int ALIGN>
class AlignedMalloc
{
public:
    void* operator new (std::size_t size)
    {
        void* ptr = aligned_malloc<ALIGN>(size);
        if (!ptr)
            throw std::bad_alloc();
        return ptr;
    }

    void* operator new (std::size_t size, const std::nothrow_t& nothrow_value) noexcept
    {
        void* ptr = aligned_malloc<ALIGN>(size);
        return ptr;
    }

    void* operator new (std::size_t size, void* ptr) noexcept
    {
    }

    void operator delete (void* ptr) noexcept
    {
        aligned_free<ALIGN>(ptr);
    }

    void operator delete (void* ptr, const std::nothrow_t& nothrow_constant) noexcept
    {
        aligned_free<ALIGN>(ptr);
    }

    void operator delete (void* ptr, void* ptr2) noexcept
    {
    }
};

} // namespace treecore

#endif // ztd_aligned_malloc_h__
