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

#ifndef JUCE_PROCESS_H_INCLUDED
#define JUCE_PROCESS_H_INCLUDED

#include "treecore/ClassUtils.h"
#include "treecore/IntTypes.h"
#include "treecore/PlatformDefs.h"

namespace treecore {

class String;
class StringArray;

#if TREECORE_OS_WINDOWS
void TREECORE_STDCALL repeatLastProcessPriority();
bool TREECORE_STDCALL isRunningInWine();
#endif

//==============================================================================
/** Represents the current executable's process.

    This contains methods for controlling the current application at the
    process-level.

    @see Thread
 */
class TREECORE_SHARED_API Process
{
public:
    //==============================================================================
    enum ProcessPriority
    {
        LowPriority = 0,
        NormalPriority   = 1,
        HighPriority     = 2,
        RealtimePriority = 3
    };

    /** Changes the current process's priority.

        @param priority     the process priority, where
                            0=low, 1=normal, 2=high, 3=realtime
     */
    static void TREECORE_STDCALL setPriority( const ProcessPriority priority );

    /** Kills the current process immediately.

        This is an emergency process terminator that kills the application
        immediately - it's intended only for use only when something goes
        horribly wrong.
     */
    static void TREECORE_STDCALL terminate();

    /** Raises the current process's privilege level.

        Does nothing if this isn't supported by the current OS, or if process
        privilege level is fixed.
     */
    static void TREECORE_STDCALL raisePrivilege();

    /** Lowers the current process's privilege level.

        Does nothing if this isn't supported by the current OS, or if process
        privilege level is fixed.
     */
    static void TREECORE_STDCALL lowerPrivilege();

    //==============================================================================
    /** Returns true if this process is being hosted by a debugger. */
    static bool TREECORE_STDCALL isRunningUnderDebugger();

    static int32 TREECORE_STDCALL getProcessID();
    //==============================================================================
    /** Tries to launch the OS's default reader application for a given file or URL. */
    static bool TREECORE_STDCALL openDocument( const String& documentURL, const String& parameters );

    /** Tries to launch the OS's default email application to let the user create a message. */
    static bool TREECORE_STDCALL openEmailWithAttachments( const String&      targetEmailAddress,
                                                           const String&      emailSubject,
                                                           const String&      bodyText,
                                                           const StringArray& filesToAttach );

#if TREECORE_OS_WINDOWS || TREECORE_COMPILER_DOXYGEN
    /** WINDOWS ONLY - This returns the HINSTANCE of the current module.

        The return type is a void* to avoid being dependent on windows.h - just cast
        it to a HINSTANCE to use it.

        In a normal application, this will be automatically set to the module
        handle of the executable.

        If you've built a DLL and plan to use any messaging or windowing classes,
        you'll need to make sure you call the setCurrentModuleInstanceHandle()
        to provide the correct module handle in your DllMain() function, because
        the system relies on the correct instance handle when opening windows.
     */
    static void* TREECORE_STDCALL getCurrentModuleInstanceHandle() noexcept;

    /** WINDOWS ONLY - Sets a new module handle to be used by the library.

        The parameter type is a void* to avoid being dependent on windows.h, but it actually
        expects a HINSTANCE value.

        @see getCurrentModuleInstanceHandle()
     */
    static void TREECORE_STDCALL setCurrentModuleInstanceHandle( void* newHandle ) noexcept;
#endif

#if TREECORE_OS_OSX || TREECORE_COMPILER_DOXYGEN
    /** OSX ONLY - Shows or hides the OSX dock icon for this app. */
    static void setDockIconVisible( bool isVisible );
#endif

private:
    Process();
    TREECORE_DECLARE_NON_COPYABLE( Process )
};

}

#endif   // JUCE_PROCESS_H_INCLUDED
