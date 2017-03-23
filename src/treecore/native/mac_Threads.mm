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

/*
    Note that a lot of methods that you'd expect to find in this file actually
    live in juce_posix_SharedCode.h!
 */

#include "treecore/DebugUtils.h"
#include "treecore/PlatformDefs.h"
#include "treecore/Process.h"
#include "treecore/Thread.h"

#include "treecore/native/osx_ObjCHelpers.h"

#include <sys/types.h>
#include <sys/sysctl.h>
#include <errno.h>

namespace treecore
{

#if TREECORE_OS_IOS
bool isIOSAppActive = true;
#endif

//==============================================================================

TREECORE_SHARED_API void TREECORE_STDCALL Process::raisePrivilege()
{
    treecore_assert_false;
}

TREECORE_SHARED_API void TREECORE_STDCALL Process::lowerPrivilege()
{
    treecore_assert_false;
}

TREECORE_SHARED_API void TREECORE_STDCALL Process::setPriority( ProcessPriority )
{
    // xxx
}

TREECORE_SHARED_API bool TREECORE_STDCALL Process::isRunningUnderDebugger()
{
    static char testResult = 0;

    if (testResult == 0)
    {
        struct kinfo_proc info;
        int m[]   = { CTL_KERN, KERN_PROC, KERN_PROC_PID, getpid() };
        size_t sz = sizeof(info);
        sysctl( m, 4, &info, &sz, 0, 0 );
        testResult = ( (info.kp_proc.p_flag & P_TRACED) != 0 ) ? 1 : -1;
    }

    return testResult > 0;
}
    
    
void TREECORE_STDCALL Thread::setCurrentThreadName( const String& name )
{
    TREECORE_AUTO_RELEASE_POOL
    {
        [[NSThread currentThread] setName : juceStringToNS( name )];
    }
}
    
} // namespace treecore
