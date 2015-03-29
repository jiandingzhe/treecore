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

#ifndef JUCE_STANDARDHEADER_H_INCLUDED
#define JUCE_STANDARDHEADER_H_INCLUDED

//==============================================================================
/** Current JUCE version number.

    See also SystemStats::getJUCEVersion() for a string version.
*/
#define JUCE_MAJOR_VERSION      3
#define JUCE_MINOR_VERSION      0
#define JUCE_BUILDNUMBER        8

/** Current Juce version number.

    Bits 16 to 32 = major version.
    Bits 8 to 16 = minor version.
    Bits 0 to 8 = point release.

    See also SystemStats::getJUCEVersion() for a string version.
*/
#define JUCE_VERSION   ((JUCE_MAJOR_VERSION << 16) + (JUCE_MINOR_VERSION << 8) + JUCE_BUILDNUMBER)


//==============================================================================
#include "treejuce/PlatformDefs.h"

//==============================================================================
// Now we'll include some common OS headers..
#if defined TREEJUCE_COMPILER_MSVC
#   pragma warning (push)
#   pragma warning (disable: 4514 4245 4100)
#endif

// to use rand_s under MSVC, this macro must be defined before first inclusion of stdlib.h
#ifdef TREEJUCE_COMPILER_MSVC
#  define _CRT_RAND_S
#endif

#include <cstdlib>
#include <cstdint>
#include <cstdarg>
#include <climits>
#include <limits>
#include <cmath>
#include <cwchar>
#include <stdexcept>
#include <typeinfo>
#include <cstring>
#include <cstdio>
#include <iostream>
#include <vector>
#include <algorithm>

#if JUCE_USE_INTRINSICS
#   include <intrin.h>
#endif

#if defined TREEJUCE_OS_OSX || defined TREEJUCE_OS_IOS
#   include <libkern/OSAtomic.h>
#endif

#if defined TREEJUCE_OS_LINUX
#   include <signal.h>
#   if defined TREEJUCE_COMPILER_ICC
#       if TREEJUCE_SIZE_PTR == 8
#           include <ia64intrin.h>
#       elif TREEJUCE_SIZE_PTR == 4
#           include <ia32intrin.h>
#       else
#           error "unsupported pointer size"
#       endif
#   endif
#endif

#if defined TREEJUCE_COMPILER_MSVC && JUCE_DEBUG
#   include <crtdbg.h>
#endif

#if defined TREEJUCE_COMPILER_MSVC
#   pragma warning (pop)
#endif

#if defined TREEJUCE_OS_ANDROID
#   include <sys/atomics.h>
#   include <byteswap.h>
#endif

// undef symbols that are sometimes set by misguided 3rd-party headers..
#undef check
#undef TYPE_BOOL
#undef max
#undef min

//==============================================================================
// DLL building settings on Windows
#if defined TREEJUCE_COMPILER_MSVC
#   ifdef JUCE_DLL_BUILD
#       define JUCE_API __declspec (dllexport)
#       pragma warning (disable: 4251)
#   elif defined (JUCE_DLL)
#       define JUCE_API __declspec (dllimport)
#       pragma warning (disable: 4251)
#   endif
#   ifdef TREEJUCE_COMPILER_ICC
#       pragma warning (disable: 1125) // (virtual override warning)
#   endif
#elif defined (JUCE_DLL) || defined (JUCE_DLL_BUILD)
#   define JUCE_API __attribute__ ((visibility("default")))
#endif

//==============================================================================
#ifndef JUCE_API
#   define JUCE_API   /**< This macro is added to all juce public class declarations. */
#endif

#if defined TREEJUCE_COMPILER_MSVC && JUCE_DLL_BUILD
#   define JUCE_PUBLIC_IN_DLL_BUILD(declaration)  public: declaration; private:
#else
#   define JUCE_PUBLIC_IN_DLL_BUILD(declaration)  declaration;
#endif

/** This macro is added to all juce public function declarations. */
#define JUCE_PUBLIC_FUNCTION        JUCE_API JUCE_CALLTYPE

#if (! defined (JUCE_CATCH_DEPRECATED_CODE_MISUSE)) && JUCE_DEBUG && ! DOXYGEN
 /** This turns on some non-essential bits of code that should prevent old code from compiling
     in cases where method signatures have changed, etc.
 */
#   define JUCE_CATCH_DEPRECATED_CODE_MISUSE 1
#endif

TREEFACE_JUCE_NAMESPACE_BEGIN
extern JUCE_API bool JUCE_CALLTYPE juce_isRunningUnderDebugger();
extern JUCE_API void JUCE_CALLTYPE logAssertion (const char* file, int line) NOEXCEPT;
TREEFACE_JUCE_NAMESPACE_END

#endif   // JUCE_STANDARDHEADER_H_INCLUDED
