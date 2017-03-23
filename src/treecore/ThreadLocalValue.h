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

#ifndef TREECORE_THREADLOCALVALUE_H
#define TREECORE_THREADLOCALVALUE_H

#include "treecore/AtomicFunc.h"

#include "treecore/SpinLock.h"
#include "treecore/Thread.h"

namespace treecore {

/**
    Provides cross-platform support for thread-local objects.

    This class holds an internal list of objects of the templated type, keeping
    an instance for each thread that requests one. The first time a thread attempts
    to access its value, an object is created and added to the list for that thread.

    Typically, you'll probably want to create a static instance of a ThreadLocalValue
    object, or hold one within a singleton.

    The templated class for your value must be a primitive type, or a simple POD struct.

    When a thread no longer needs to use its value, it can call releaseCurrentThreadStorage()
    to allow the storage to be re-used by another thread. If a thread exits without calling
    this method, the object storage will be left allocated until the ThreadLocalValue object
    is deleted.
 */
#ifdef NEVER_TOUCH
template<typename Type>
class ThreadLocalValue
{
public:
    /** */
    ThreadLocalValue() noexcept
    {}

    /** Destructor.
        When this object is deleted, all the value objects for all threads will be deleted.
     */
    ~ThreadLocalValue()
    {
    }

    /** Returns a reference to this thread's instance of the value.
        Note that the first time a thread tries to access the value, an instance of the
        value object will be created - so if your value's class has a non-trivial
        constructor, be aware that this method could invoke it.
     */
    Type& operator * () const noexcept                        { return get(); }

    /** Returns a pointer to this thread's instance of the value.
        Note that the first time a thread tries to access the value, an instance of the
        value object will be created - so if your value's class has a non-trivial
        constructor, be aware that this method could invoke it.
     */
    operator Type* () const noexcept{ return &get(); }

    /** Accesses a method or field of the value object.
        Note that the first time a thread tries to access the value, an instance of the
        value object will be created - so if your value's class has a non-trivial
        constructor, be aware that this method could invoke it.
     */
    Type* operator -> () const noexcept                       { return &get(); }

    /** Assigns a new value to the thread-local object. */
    ThreadLocalValue& operator = ( const Type& newValue )      { get() = newValue; return *this; }

    /** Returns a reference to this thread's instance of the value.
        Note that the first time a thread tries to access the value, an instance of the
        value object will be created - so if your value's class has a non-trivial
        constructor, be aware that this method could invoke it.
     */
    Type& get() const noexcept
    {
        static thread_local Type object;
        return object;
    }

    /** Called by a thread before it terminates, to allow this class to release
        any storage associated with the thread.
     */
    void releaseCurrentThreadStorage()
    {
    }

private:

    TREECORE_DECLARE_NON_COPYABLE( ThreadLocalValue )
};
#endif // TREECORE_OS_OSX || TREECORE_OS_IOS
} // namespace treecore

#endif   // TREECORE_THREADLOCALVALUE_H
