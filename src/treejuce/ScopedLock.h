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

#ifndef JUCE_SCOPEDLOCK_H_INCLUDED
#define JUCE_SCOPEDLOCK_H_INCLUDED

#include "treejuce/StandardHeader.h"

namespace treejuce {

//==============================================================================
/**
    Automatically locks and unlocks a mutex object.

    Use one of these as a local variable to provide RAII-based locking of a mutex.

    The templated class could be a CriticalSection, SpinLock, or anything else that
    provides enter() and exit() methods.

    e.g. @code
    CriticalSection myCriticalSection;

    for (;;)
    {
        const GenericScopedLock<CriticalSection> myScopedLock (myCriticalSection);
        // myCriticalSection is now locked

        ...do some stuff...

        // myCriticalSection gets unlocked here.
    }
    @endcode

    @see GenericScopedUnlock, CriticalSection, SpinLock, ScopedLock, ScopedUnlock
*/
template <class LockType>
class GenericScopedLock
{
public:
    //==============================================================================
    /** Creates a GenericScopedLock.

        As soon as it is created, this will acquire the lock, and when the GenericScopedLock
        object is deleted, the lock will be released.

        Make sure this object is created and deleted by the same thread,
        otherwise there are no guarantees what will happen! Best just to use it
        as a local stack object, rather than creating one with the new() operator.
    */
    inline explicit GenericScopedLock (const LockType& lock) NOEXCEPT : lock_ (lock)     { lock.enter(); }

    /** Destructor.
        The lock will be released when the destructor is called.
        Make sure this object is created and deleted by the same thread, otherwise there are
        no guarantees what will happen!
    */
    inline ~GenericScopedLock() NOEXCEPT                                                 { lock_.exit(); }

private:
    //==============================================================================
    const LockType& lock_;

    JUCE_DECLARE_NON_COPYABLE (GenericScopedLock)
};


//==============================================================================
/**
    Automatically unlocks and re-locks a mutex object.

    This is the reverse of a GenericScopedLock object - instead of locking the mutex
    for the lifetime of this object, it unlocks it.

    Make sure you don't try to unlock mutexes that aren't actually locked!

    e.g. @code

    CriticalSection myCriticalSection;

    for (;;)
    {
        const GenericScopedLock<CriticalSection> myScopedLock (myCriticalSection);
        // myCriticalSection is now locked

        ... do some stuff with it locked ..

        while (xyz)
        {
            ... do some stuff with it locked ..

            const GenericScopedUnlock<CriticalSection> unlocker (myCriticalSection);

            // myCriticalSection is now unlocked for the remainder of this block,
            // and re-locked at the end.

            ...do some stuff with it unlocked ...
        }

        // myCriticalSection gets unlocked here.
    }
    @endcode

    @see GenericScopedLock, CriticalSection, ScopedLock, ScopedUnlock
*/
template <class LockType>
class GenericScopedUnlock
{
public:
    //==============================================================================
    /** Creates a GenericScopedUnlock.

        As soon as it is created, this will unlock the CriticalSection, and
        when the ScopedLock object is deleted, the CriticalSection will
        be re-locked.

        Make sure this object is created and deleted by the same thread,
        otherwise there are no guarantees what will happen! Best just to use it
        as a local stack object, rather than creating one with the new() operator.
    */
    inline explicit GenericScopedUnlock (const LockType& lock) NOEXCEPT : lock_ (lock)   { lock.exit(); }

    /** Destructor.

        The CriticalSection will be unlocked when the destructor is called.

        Make sure this object is created and deleted by the same thread,
        otherwise there are no guarantees what will happen!
    */
    inline ~GenericScopedUnlock() NOEXCEPT                                               { lock_.enter(); }


private:
    //==============================================================================
    const LockType& lock_;

    JUCE_DECLARE_NON_COPYABLE (GenericScopedUnlock)
};


//==============================================================================
/**
    Automatically locks and unlocks a mutex object.

    Use one of these as a local variable to provide RAII-based locking of a mutex.

    The templated class could be a CriticalSection, SpinLock, or anything else that
    provides enter() and exit() methods.

    e.g. @code

    CriticalSection myCriticalSection;

    for (;;)
    {
        const GenericScopedTryLock<CriticalSection> myScopedTryLock (myCriticalSection);

        // Unlike using a ScopedLock, this may fail to actually get the lock, so you
        // should test this with the isLocked() method before doing your thread-unsafe
        // action..
        if (myScopedTryLock.isLocked())
        {
           ...do some stuff...
        }
        else
        {
            ..our attempt at locking failed because another thread had already locked it..
        }

        // myCriticalSection gets unlocked here (if it was locked)
    }
    @endcode

    @see CriticalSection::tryEnter, GenericScopedLock, GenericScopedUnlock
*/
template <class LockType>
class GenericScopedTryLock
{
public:
    //==============================================================================
    /** Creates a GenericScopedTryLock.

        As soon as it is created, this will attempt to acquire the lock, and when the
        GenericScopedTryLock is deleted, the lock will be released (if the lock was
        successfully acquired).

        Make sure this object is created and deleted by the same thread,
        otherwise there are no guarantees what will happen! Best just to use it
        as a local stack object, rather than creating one with the new() operator.
    */
    inline explicit GenericScopedTryLock (const LockType& lock) NOEXCEPT
        : lock_ (lock), lockWasSuccessful (lock.tryEnter()) {}

    /** Destructor.

        The mutex will be unlocked (if it had been successfully locked) when the
        destructor is called.

        Make sure this object is created and deleted by the same thread,
        otherwise there are no guarantees what will happen!
    */
    inline ~GenericScopedTryLock() NOEXCEPT         { if (lockWasSuccessful) lock_.exit(); }

    /** Returns true if the mutex was successfully locked. */
    bool isLocked() const NOEXCEPT                  { return lockWasSuccessful; }

private:
    //==============================================================================
    const LockType& lock_;
    const bool lockWasSuccessful;

    JUCE_DECLARE_NON_COPYABLE (GenericScopedTryLock)
};

}

#endif   // JUCE_SCOPEDLOCK_H_INCLUDED
