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

#include "treecore/RefCountHolder.h"
#include "treecore/Logger.h"
#include "treecore/RefCountObject.h"
#include "treecore/Time.h"
#include "treecore/Thread.h"
#include "treecore/ThreadLocalValue.h"

namespace treecore {

Thread::Thread ( const String& threadName_ )
    : threadName( threadName_ ),
    threadHandle( nullptr ),
    threadId( 0 ),
    threadPriority( 5 ),
    affinityMask( 0 ),
    shouldExit( false )
{}

Thread::~Thread()
{
    /* If your thread class's destructor has been called without first stopping the thread, that
       means that this partially destructed object is still performing some work - and that's
       probably a Bad Thing!

       To avoid this type of nastiness, always make sure you call stopThread() before or during
       your subclass's destructor.
     */
    treecore_assert( !isThreadRunning() );

    stopThread( -1 );
}

thread_local Thread* _current_thread_;

void Thread::threadEntryPoint()
{
    _current_thread_ = this;

    if ( threadName.isNotEmpty() )
        setCurrentThreadName( threadName );

    if ( startSuspensionEvent.wait( 10000 ) )
    {
        treecore_assert( getCurrentThreadId() == threadId );

        if (affinityMask != 0)
            setCurrentThreadAffinityMask( affinityMask );

        run();
    }

    closeThreadHandle();
}

// used to wrap the incoming call from the platform-specific code
void TREECORE_SHARED_API _thread_entry_point_( void* userData )
{
    static_cast<Thread*>(userData)->threadEntryPoint();
}

//==============================================================================
void Thread::startThread()
{
    const ScopedLock sl( startStopLock );

    shouldExit = false;

    if (threadHandle == nullptr)
    {
        launchThread();
        setThreadPriority( threadHandle, threadPriority );
        startSuspensionEvent.signal();
    }
}

void Thread::startThread( const int priority )
{
    const ScopedLock sl( startStopLock );

    if (threadHandle == nullptr)
    {
        threadPriority = priority;
        startThread();
    }
    else
    {
        setPriority( priority );
    }
}

bool Thread::isThreadRunning() const
{
    return threadHandle != nullptr;
}

Thread* TREECORE_STDCALL Thread::getCurrentThread()
{
    return _current_thread_;
}

//==============================================================================
void Thread::signalThreadShouldExit()
{
    shouldExit = true;
}

bool Thread::waitForThreadToExit( const int timeOutMilliseconds ) const
{
    // Doh! So how exactly do you expect this thread to wait for itself to stop??
    treecore_assert( getThreadId() != getCurrentThreadId() || getCurrentThreadId() == 0 );

    const uint32 timeoutEnd = Time::getMillisecondCounter() + (uint32) timeOutMilliseconds;

    while ( isThreadRunning() )
    {
        if (timeOutMilliseconds >= 0 && Time::getMillisecondCounter() > timeoutEnd)
            return false;

        sleep( 2 );
    }

    return true;
}

bool Thread::stopThread( const int timeOutMilliseconds )
{
    // agh! You can't stop the thread that's calling this method! How on earth
    // would that work??
    treecore_assert( getCurrentThreadId() != getThreadId() );

    const ScopedLock sl( startStopLock );

    if ( isThreadRunning() )
    {
        signalThreadShouldExit();
        notify();

        if (timeOutMilliseconds != 0)
            waitForThreadToExit( timeOutMilliseconds );

        if ( isThreadRunning() )
        {
            // very bad karma if this point is reached, as there are bound to be
            // locks and events left in silly states when a thread is killed by force..
            treecore_assert_false;
            Logger::writeToLog( "!! killing thread by force !!" );

            killThread();

            threadHandle = nullptr;
            threadId = 0;
            return false;
        }
    }

    return true;
}

//==============================================================================
bool Thread::setPriority( const int newPriority )
{
    // NB: deadlock possible if you try to set the thread prio from the thread itself,
    // so using setCurrentThreadPriority instead in that case.
    if ( getCurrentThreadId() == getThreadId() )
        return setCurrentThreadPriority( newPriority );

    const ScopedLock sl( startStopLock );

    if ( ( !isThreadRunning() ) || setThreadPriority( threadHandle, newPriority ) )
    {
        threadPriority = newPriority;
        return true;
    }

    return false;
}

bool Thread::setCurrentThreadPriority( const int newPriority )
{
    return setThreadPriority( 0, newPriority );
}

void Thread::setAffinityMask( const uint32 newAffinityMask )
{
    affinityMask = newAffinityMask;
}

//==============================================================================
bool Thread::wait( const int timeOutMilliseconds ) const
{
    return defaultEvent.wait( timeOutMilliseconds );
}

void Thread::notify() const
{
    defaultEvent.signal();
}

//==============================================================================
void SpinLock::enter() const noexcept
{
    if ( !tryEnter() )
    {
        for (int i = 20; --i >= 0; )
            if ( tryEnter() )
                return;

        while ( !tryEnter() )
            Thread::yield();
    }
}

} // namespace treecore
