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

#ifndef JUCE_MEMORY_H_INCLUDED
#define JUCE_MEMORY_H_INCLUDED

#include "treecore/IntTypes.h"
#include "treecore/PlatformDefs.h"
#include <cstring>
//==============================================================================
namespace treecore {

/** Fills a block of memory with zeros. */
inline void zeromem( void* memory, size_t numBytes ) noexcept        { std::memset( memory, 0, numBytes ); }

/** Overwrites a structure or object with zeros. */
template<typename Type>
inline void zerostruct( Type& structure ) noexcept                   { std::memset( &structure, 0, sizeof(structure) ); }

/** Delete an object pointer, and sets the pointer to null.

    Remember that it's not good c++ practice to use delete directly - always try to use a ScopedPointer
    or other automatic lifetime-management system rather than resorting to deleting raw pointers!
 */
template<typename Type>
inline void deleteAndZero( Type& pointer )                           { delete pointer; pointer = nullptr; }

/** A handy function which adds a number of bytes to any type of pointer and returns the result.
    This can be useful to avoid casting pointers to a char* and back when you want to move them by
    a specific number of bytes,
 */
template<typename Type, typename IntegerType>
inline Type* addBytesToPointer( Type* pointer, IntegerType bytes ) noexcept  { return (Type*) ( ( (char*) pointer ) + bytes ); }

/** A handy function which returns the difference between any two pointers, in bytes.
    The address of the second pointer is subtracted from the first, and the difference in bytes is returned.
 */
template<typename Type1, typename Type2>
inline int getAddressDifference( Type1* pointer1, Type2* pointer2 ) noexcept  { return int( ( (const char*) pointer1 ) - (const char*) pointer2 ); }

/** If a pointer is non-null, this returns a new copy of the object that it points to, or safely returns
    nullptr if the pointer is null.
 */
template<class Type>
inline Type* createCopyIfNotNull( const Type* pointer )     { return pointer != nullptr ? new Type( *pointer ) : nullptr; }

//==============================================================================
#if TREECORE_OS_OSX || TREECORE_OS_IOS || TREECORE_COMPILER_DOXYGEN

/** A handy C++ wrapper that creates and deletes an NSAutoreleasePool object using RAII.
     You should use the TREECORE_AUTO_RELEASE_POOL macro to create a local auto-release pool on the stack.
 */
class TREECORE_SHARED_API ScopedAutoReleasePool
{
public:
    ScopedAutoReleasePool();
    ~ScopedAutoReleasePool();

private:
    void* pool;

    TREECORE_DECLARE_NON_COPYABLE( ScopedAutoReleasePool )
};

/** A macro that can be used to easily declare a local ScopedAutoReleasePool
     object for RAII-based obj-C autoreleasing.
     Because this may use the \@autoreleasepool syntax, you must follow the macro with
     a set of braces to mark the scope of the pool.
 */
#    if defined __OBJC__ || DOXYGEN
#        define TREECORE_AUTO_RELEASE_POOL  @autoreleasepool
#    else
#        define TREECORE_AUTO_RELEASE_POOL  const treecore::ScopedAutoReleasePool TREECORE_STRINGIFY_JOIN( autoReleasePool_, __LINE__ );
#    endif

#else
#    define TREECORE_AUTO_RELEASE_POOL
#endif

//==============================================================================
/* In a Windows DLL build, we'll expose some malloc/free functions that live inside the DLL, and use these for
   allocating all the objects - that way all objects in the DLL and in the host will live in the same heap,
   avoiding problems when an object is created in one module and passed across to another where it is deleted.
   By piggy-backing on the TREECORE_LEAK_DETECTOR macro, these allocators can be injected into most classes.
 */
#if TREECORE_COMPILER_MSVC && (TREECORE_USING_DLL || TREECORE_DLL_BUILD) && !(TREECORE_DISABLE_DLL_ALLOCATORS || DOXYGEN)
extern TREECORE_SHARED_API void* _dll_malloc_( size_t );
extern TREECORE_SHARED_API void  _dll_free_( void* );

#    define TREECORE_LEAK_DETECTOR( OwnerClass )  public:                                            \
    static void* operator new ( size_t sz )           { return treecore::_dll_malloc_( sz ); } \
    static void* operator new ( size_t, void* p )     { return p; }                              \
    static void operator delete ( void* p )           { treecore::_dll_free_( p ); }           \
    static void operator delete ( void*, void* )      {}
#endif

#define checkPtrSIMD( ptr, size ) treecore_assert( ( uintptr_t(ptr) ) % size == 0 );
#define checkPtrSIMD_nonNullptr( ptr, size ) checkPtrSIMD( ptr, size ); treecore_assert( ptr != nullptr );
#define checkSizeSIMD( x, size ) treecore_assert( x % size == 0 );
#define checkPtrSIMD_nonZero( x, size ) checkSizeSIMD( x, size ); treecore_assert( x > 0 );

}

#endif   // JUCE_MEMORY_H_INCLUDED
