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

#ifndef JUCE_SCOPEDPOINTER_H_INCLUDED
#define JUCE_SCOPEDPOINTER_H_INCLUDED

#include "treecore/ContainerDeletePolicy.h"
#include "treecore/LeakedObjectDetector.h"

namespace treecore {


template < typename T >
class UniPtr
{
public:
    UniPtr() = default;
    
    forcedinline UniPtr (T* const objectToTakePossessionOf) noexcept
        : object (objectToTakePossessionOf)
    {}

    forcedinline UniPtr( UniPtr&& other ) noexcept
        : UniPtr()
    {
        this->operator=( std::move( other ) );
    }

    forcedinline UniPtr& operator= ( UniPtr&& other ) noexcept
    {
        object = other.object;
        other.object = nullptr;
        return *this;
    }

    forcedinline ~UniPtr() { ContainerDeletePolicy<T >::destroy( object ); }

    /** Changes this UniPtr to point to a new object.

        Because a pointer can only belong to one UniPtr, this transfers
        the pointer from the other object to this one, and the other object is reset to
        be a null pointer.

        If this UniPtr already points to an object, that object
        will first be deleted.
    */
    UniPtr& operator= (UniPtr& objectToTransferFrom)
    {
        if (this != objectToTransferFrom.getAddress())
        {
            // Two UniPtrs should never be able to refer to the same object - if
            // this happens, you must have done something dodgy!
            tassert (object == nullptr || object != objectToTransferFrom.object);

            T* const oldObject = object;
            object = objectToTransferFrom.object;
            objectToTransferFrom.object = nullptr;
            ContainerDeletePolicy<T >::destroy (oldObject);
        }

        return *this;
    }

    /** Changes this UniPtr to point to a new object.

        If this UniPtr already points to an object, that object
        will first be deleted.

        The pointer that you pass in may be a nullptr.
    */
    UniPtr& operator= (T* const newObjectToTakePossessionOf)
    {
        if (object != newObjectToTakePossessionOf)
        {
            T* const oldObject = object;
            object = newObjectToTakePossessionOf;
            ContainerDeletePolicy<T >::destroy (oldObject);
        }

        return *this;
    }

    forcedinline operator T*() const noexcept   { return object; }
    forcedinline T* get() const noexcept        { return object; }
    forcedinline T& operator*() const noexcept  { return *object; }
    forcedinline T* operator->() const noexcept { return object; }

    T* release() noexcept { T* const o = object; object = nullptr; return o; }

    void swapWith( UniPtr<T >& other ) noexcept
    {
        // Two UniPtrs should never be able to refer to the same object - if
        // this happens, you must have done something dodgy!
        tassert( object != other.object || this == other.getAddress() );
        std::swap (object, other.object);
    }

private:
    T* object = nullptr;

    // (Required as an alternative to the overloaded & operator).
    const UniPtr* getAddress() const noexcept { return this; }

  #if !defined TREECORE_COMPILER_MSVC // (MSVC can't deal with multiple copy constructors)
    /* The copy constructors are private to stop people accidentally copying a const UniPtr
       (the compiler would let you do so by implicitly casting the source to its raw object pointer).

       A side effect of this is that in a compiler that doesn't support C++11, you may hit an
       error when you write something like this:

          UniPtr<MyClass> m = new MyClass();  // Compile error: copy constructor is private.

       Even though the compiler would normally ignore the assignment here, it can't do so when the
       copy constructor is private. It's very easy to fix though - just write it like this:

          UniPtr<MyClass> m (new MyClass());  // Compiles OK

       It's probably best to use the latter form when writing your object declarations anyway, as
       this is a better representation of the code that you actually want the compiler to produce.
    */
    TREECORE_DECLARE_NON_COPYABLE (UniPtr)
  #endif
};

#define COMPARE_FUN(Operate)\
template< typename T > \
bool operator== ( const UniPtr<T>& pointer1 , T* const pointer2 ) noexcept \
{\
    return static_cast<T*>(pointer1) Operate pointer2;\
}\

COMPARE_FUN( == );
COMPARE_FUN( != );
COMPARE_FUN( > );
COMPARE_FUN( >= );
COMPARE_FUN( < );
COMPARE_FUN( <= );

#undef COMPARE_FUN

//==============================================================================
#ifndef DOXYGEN
// NB: This is just here to prevent any silly attempts to call deleteAndZero() on a UniPtr.
template <typename Type>
void deleteAndZero( UniPtr<Type>& ) { static_assert ( sizeof( Type ) == 12345 ); }
#endif

}

#endif   // JUCE_SCOPEDPOINTER_H_INCLUDED
