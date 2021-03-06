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

#ifndef TREECORE_LEAKED_OBJECT_DETECTOR_H
#define TREECORE_LEAKED_OBJECT_DETECTOR_H

#include "treecore/AtomicFunc.h"
#include "treecore/DebugUtils.h"
#include "treecore/Logger.h"
#include "treecore/String.h"

#if TREECORE_DEBUG && !defined TREECORE_CHECK_MEMORY_LEAKS
#    define TREECORE_CHECK_MEMORY_LEAKS 1
#endif

namespace treecore {

/**
    Embedding an instance of this class inside another class can be used as a low-overhead
    way of detecting leaked instances.

    This class keeps an internal static count of the number of instances that are
    active, so that when the app is shutdown and the static destructors are called,
    it can check whether there are any left-over instances that may have been leaked.

    To use it, use the TREECORE_LEAK_DETECTOR macro as a simple way to put one in your
    class declaration. Have a look through the codebase for examples, it's used
    in most of the classes.
 */
template<class OwnerClass>
class LeakedObjectDetector
{
public:
    //==============================================================================
    LeakedObjectDetector() noexcept
    {
        treecore::atomic_fetch_add( &getCounter().numObjects, 1 );
    }

    LeakedObjectDetector ( const LeakedObjectDetector& ) noexcept
    {
        treecore::atomic_fetch_add( &getCounter().numObjects, 1 );
    }

    ~LeakedObjectDetector()
    {
        if (treecore::atomic_sub_fetch( &getCounter().numObjects, 1 ) < 0)
        {
            TREECORE_DBG( "*** Dangling pointer deletion! Class: " << getLeakedObjectClassName() );

            /** If you hit this, then you've managed to delete more instances of this class than you've
                created.. That indicates that you're deleting some dangling pointers.

                Note that although this assertion will have been triggered during a destructor, it might
                not be this particular deletion that's at fault - the incorrect one may have happened
                at an earlier point in the program, and simply not been detected until now.

                Most errors like this are caused by using old-fashioned, non-RAII techniques for
                your object management. Tut, tut. Always, always use ScopedPointers, OwnedArrays,
                ReferenceCountedObjects, etc, and avoid the 'delete' operator at all costs!
             */
            treecore_assert_false;
        }
    }

private:
    //==============================================================================
    class LeakCounter
    {
public:
        LeakCounter() noexcept {}

        ~LeakCounter()
        {
            int n_obj = treecore::atomic_load( &numObjects );
            if (n_obj > 0)
            {
                TREECORE_DBG( "*** Leaked objects detected: " << n_obj << " instance(s) of class " << getLeakedObjectClassName() );

                /** If you hit this, then you've leaked one or more objects of the type specified by
                    the 'OwnerClass' template parameter - the name should have been printed by the line above.

                    If you're leaking, it's probably because you're using old-fashioned, non-RAII techniques for
                    your object management. Tut, tut. Always, always use ScopedPointers, OwnedArrays,
                    ReferenceCountedObjects, etc, and avoid the 'delete' operator at all costs!
                 */
                treecore_assert_false;
            }
        }

        int numObjects;
    };

    static const char* getLeakedObjectClassName()
    {
        return OwnerClass::getLeakedObjectClassName();
    }

    static LeakCounter& getCounter() noexcept
    {
        static LeakCounter counter;
        return counter;
    }
};

//==============================================================================

#if (TREECORE_COMPILER_DOXYGEN || TREECORE_CHECK_MEMORY_LEAKS)
/** This macro lets you embed a leak-detecting object inside a class.

   To use it, simply declare a TREECORE_LEAK_DETECTOR(YourClassName) inside a private section
   of the class declaration. E.g.

   @code
   class MyClass
   {
   public:
    MyClass();
    void blahBlah();

   private:
    TREECORE_LEAK_DETECTOR (MyClass)
   };
   @endcode

   @see TREECORE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR, LeakedObjectDetector
 */
#    define TREECORE_LEAK_DETECTOR( OwnerClass )                                  \
    friend class treecore::LeakedObjectDetector<OwnerClass>;                      \
    static const char* getLeakedObjectClassName() noexcept{ return #OwnerClass; } \
    treecore::LeakedObjectDetector<OwnerClass> TREECORE_STRINGIFY_JOIN( leakDetector, __LINE__ );
#else
#    define TREECORE_LEAK_DETECTOR( OwnerClass )
#endif

}

#endif   // TREECORE_LEAKED_OBJECT_DETECTOR_H
