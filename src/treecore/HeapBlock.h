/*
   ==============================================================================

   This file is part of the juce_core module of the JUCE library.
   Copyright (c) 2013 - Raw Material Software Ltd.

   Permission to use, copy, modify, and/or distribute this software for any purpose with
   or without fee is hereby granted, provided that the above copyright notice and this
   permission notice appear in all copies.

   THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH REGARD
   TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS. IN
   NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL
   DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER
   IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN
   CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.

   ------------------------------------------------------------------------------

   NOTE! This permissive ISC license applies ONLY to files within the juce_core module!
   All other JUCE modules are covered by a dual GPL/commercial license, so if you are
   using any other modules, be sure to check that you also comply with their license.

   For more details, visit www.juce.com

   ==============================================================================
 */

#ifndef TREECORE_HEAPBLOCK_H
#define TREECORE_HEAPBLOCK_H

#include "treecore/AlignedMalloc.h"
#include "treecore/DebugUtils.h"
#include "treecore/Memory.h"

#include <utility>

namespace treecore {

#if TREECORE_DEBUG

struct NumberCheck {

    void setNum( int64 num ) { m_num = num; }

    template<typename IntType>
    void check( const IntType& ii ) const
    {
        int64 i = (int64) ii;
        treecore_assert( i >= 0 && i < m_num );
    }

    template<typename IntType>
    void checkNum( const IntType& num2 ) const
    {
        int64 num = (int64) num2;
        treecore_assert( num >= 0 && num <= m_num );
    }

    void swap( NumberCheck& other )
    {
        std::swap( m_num, other.m_num );
    }

private:
    int64 m_num = 0;
};

#else

struct NumberCheck {

    void setNum( size_t ) const {}

    template<typename IntType>
    void check( const IntType& ) const
    {}

    template<typename IntType>
    void checkNum( const IntType& ) const
    {}

    void swap( NumberCheck& ) {};

};

#endif

#ifndef DOXYGEN
namespace HeapBlockHelper
{
template<bool shouldThrow>
struct ThrowOnFail { static void check( void* ) {}
};

template<>
struct ThrowOnFail<true>   { static void check( void* data ) { if (data == nullptr) throw std::bad_alloc(); }
};
}
#endif

//==============================================================================
/**
    Very simple container class to hold a pointer to some data on the heap.

    When you need to allocate some heap storage for something, always try to use
    this class instead of allocating the memory directly using malloc/free.

    A HeapBlock<char> object can be treated in pretty much exactly the same way
    as an char*, but as long as you allocate it on the stack or as a class member,
    it's almost impossible for it to leak memory.

    It also makes your code much more concise and readable than doing the same thing
    using direct allocations,

    E.g. instead of this:
    @code
        int* temp = (int*) malloc (1024 * sizeof (int));
        memcpy (temp, xyz, 1024 * sizeof (int));
        free (temp);
        temp = (int*) calloc (2048 * sizeof (int));
        temp[0] = 1234;
        memcpy (foobar, temp, 2048 * sizeof (int));
        free (temp);
    @endcode

    ..you could just write this:
    @code
        HeapBlock <int> temp (1024);
        memcpy (temp, xyz, 1024 * sizeof (int));
        temp.calloc (2048);
        temp[0] = 1234;
        memcpy (foobar, temp, 2048 * sizeof (int));
    @endcode

    The class is extremely lightweight, containing only a pointer to the
    data, and exposes malloc/realloc/calloc/free methods that do the same jobs
    as their less object-oriented counterparts. Despite adding safety, you probably
    won't sacrifice any performance by using this in place of normal pointers.

    The throwOnFailure template parameter can be set to true if you'd like the class
    to throw a std::bad_alloc exception when an allocation fails. If this is false,
    then a failed allocation will just leave the heapblock with a null pointer (assuming
    that the system's malloc() function doesn't throw).

    @see Array, OwnedArray, MemoryBlock
 */
template<class ElementType, size_t AlignSize = 0>
class HeapBlock
{
public:
    //==============================================================================
    /** Creates a HeapBlock which is initially just a null pointer.

        After creation, you can resize the array using the malloc(), calloc(),
        or realloc() methods.
     */
    HeapBlock() noexcept: data( nullptr )
    {
        m_checker.setNum( 0 );
    }

    /** Creates a HeapBlock containing a number of elements.

        The contents of the block are undefined, as it will have been created by a
        malloc call.

        If you want an array of zero values, you can use the calloc() method or the
        other constructor that takes an InitialisationState parameter.
     */
    explicit HeapBlock ( const size_t numElements )
        : data( static_cast<ElementType*>( aligned_malloc<AlignSize>( numElements * sizeof(ElementType) ) ) )
    {
        m_checker.setNum( numElements );
    }

    /** Creates a HeapBlock containing a number of elements.

        The initialiseToZero parameter determines whether the new memory should be cleared,
        or left uninitialised.
     */
    HeapBlock ( const size_t numElements, const bool initialiseToZero )
        : data( static_cast<ElementType*>( initialiseToZero
                                           ? aligned_calloc<AlignSize>( numElements * sizeof(ElementType) )
                                           : aligned_malloc<AlignSize>( numElements * sizeof(ElementType) ) ) )
    {
        m_checker.setNum( numElements );
    }

    /** Destructor.
        This will free the data, if any has been allocated.
     */
    ~HeapBlock()
    {
        m_checker.setNum( 0 );
        aligned_free<AlignSize>( data );
    }

    HeapBlock ( HeapBlock&& other ) noexcept
        : data( other.data )
    {
        m_checker.swap( other.m_checker );
        other.data = nullptr;
    }

    HeapBlock& operator = ( HeapBlock&& other ) noexcept
    {
        m_checker.swap( other.m_checker );
        std::swap( data, other.data );
        return *this;
    }

    //==============================================================================
    /** Returns a raw pointer to the allocated data.
        This may be a null pointer if the data hasn't yet been allocated, or if it has been
        freed by calling the free() method.
     */
    inline operator ElementType* () const noexcept                           { return data; }

    /** Returns a raw pointer to the allocated data.
        This may be a null pointer if the data hasn't yet been allocated, or if it has been
        freed by calling the free() method.
     */
    inline ElementType* getData() const noexcept                            { return data; }

    /** Returns a void pointer to the allocated data.
        This may be a null pointer if the data hasn't yet been allocated, or if it has been
        freed by calling the free() method.
     */
    inline operator void* () const noexcept                                  { return static_cast<void*>(data); }

    /** Returns a void pointer to the allocated data.
        This may be a null pointer if the data hasn't yet been allocated, or if it has been
        freed by calling the free() method.
     */
    inline operator const void* () const noexcept                            { return static_cast<const void*>(data); }

    /** Lets you use indirect calls to the first element in the array.
        Obviously this will cause problems if the array hasn't been initialised, because it'll
        be referencing a null pointer.
     */
    inline ElementType* operator -> () const noexcept                        { return data; }

    /** Returns a reference to one of the data elements.
        Obviously there's no bounds-checking here, as this object is just a dumb pointer and
        has no idea of the size it currently has allocated.
     */
    template<typename IndexType>
    inline ElementType& operator [] ( IndexType index ) const noexcept
    {
        m_checker.check( index );
        return data [index];
    }

    /** Returns a pointer to a data element at an offset from the start of the array.
        This is the same as doing pointer arithmetic on the raw pointer itself.
     */
    template<typename IndexType>
    inline ElementType* operator + ( IndexType index ) const noexcept          { m_checker.checkNum( index ); return data + index; }

    //==============================================================================
    /** Compares the pointer with another pointer.
        This can be handy for checking whether this is a null pointer.
     */
    inline bool operator == ( const ElementType* const otherPointer ) const noexcept   { return otherPointer == data; }

    /** Compares the pointer with another pointer.
        This can be handy for checking whether this is a null pointer.
     */
    inline bool operator != ( const ElementType* const otherPointer ) const noexcept   { return otherPointer != data; }

    //==============================================================================
    /** Allocates a specified amount of memory.

        This uses the normal malloc to allocate an amount of memory for this object.
        Any previously allocated memory will be freed by this method.

        The number of bytes allocated will be (newNumElements * elementSize). Normally
        you wouldn't need to specify the second parameter, but it can be handy if you need
        to allocate a size in bytes rather than in terms of the number of elements.

        The data that is allocated will be freed when this object is deleted, or when you
        call free() or any of the allocation methods.
     */
    void malloc( const size_t newNumElements, const size_t elementSize = sizeof(ElementType) )
    {
        aligned_free<AlignSize>( data );
        data = static_cast<ElementType*>( aligned_malloc<AlignSize>( newNumElements * elementSize ) );
        m_checker.setNum( newNumElements );
    }

    /** Allocates a specified amount of memory and clears it.
        This does the same job as the malloc() method, but clears the memory that it allocates.
     */
    void calloc( const size_t newNumElements, const size_t elementSize = sizeof(ElementType) )
    {
        aligned_free<AlignSize>( data );
        data = static_cast<ElementType*>( aligned_calloc<AlignSize>( newNumElements * elementSize ) );
        m_checker.setNum( newNumElements );
    }

    void allocAndSet( const size_t newNumElements, const ElementType& initValue, const size_t elementSize = sizeof( ElementType ) )
    {
        malloc( newNumElements, elementSize );
        for (size_t i = 0; i < newNumElements; ++i) {
            data[i] = initValue;
        }
    }

    /** Allocates a specified amount of memory and optionally clears it.
        This does the same job as either malloc() or calloc(), depending on the
        initialiseToZero parameter.
     */
    void allocate( const size_t newNumElements, bool initialiseToZero )
    {
        aligned_free<AlignSize>( data );
        data = static_cast<ElementType*>( initialiseToZero ? aligned_calloc<AlignSize>( newNumElements * sizeof(ElementType) )
                                          : aligned_malloc<AlignSize>( newNumElements * sizeof(ElementType) ) );
        m_checker.setNum( newNumElements );
    }

    /** Re-allocates a specified amount of memory.

        The semantics of this method are the same as malloc() and calloc(), but it
        uses realloc() to keep as much of the existing data as possible.
     */
    void realloc( const size_t newNumElements, const size_t elementSize = sizeof(ElementType) )
    {
        data = static_cast<ElementType*>( data == nullptr ? aligned_malloc<AlignSize>( newNumElements * elementSize )
                                          : aligned_realloc<AlignSize>( data, newNumElements * elementSize ) );
        m_checker.setNum( newNumElements );
    }

    /** Frees any currently-allocated data.
        This will free the data and reset this object to be a null pointer.
     */
    void free()
    {
        aligned_free<AlignSize>( data );
        m_checker.setNum( 0 );
        data = nullptr;
    }

    /** Swaps this object's data with the data of another HeapBlock.
        The two objects simply exchange their data pointers.
     */
    void swapWith( HeapBlock<ElementType>& other ) noexcept
    {
        m_checker.swap( other.m_checker );
        std::swap( data, other.data );
    }

    /** This fills the block with zeros, up to the number of elements specified.
        Since the block has no way of knowing its own size, you must make sure that the number of
        elements you specify doesn't exceed the allocated size.
     */
    void clear( size_t numElements ) noexcept
    {
        m_checker.checkNum( numElements );
        zeromem( data, sizeof(ElementType) * numElements );
    }

    /** This typedef can be used to get the type of the heapblock's elements. */
    typedef ElementType Type;

private:
    //==============================================================================
    ElementType* data;
    NumberCheck m_checker;
};

}

#endif   // TREECORE_HEAPBLOCK_H
