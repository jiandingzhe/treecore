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

#ifndef TREECORE_PLATFORM_DEFS_H
#define TREECORE_PLATFORM_DEFS_H

#include "treecore/Config.h"
#include "treecore/MiscUtils.h"

// CPU properties
#if TREECORE_CPU_X86
#    define TREECORE_ENDIAN_LITTLE 1
#    define TREECORE_ENDIAN_BIG 0
#else
#    error "unsupported CPU"
#endif

//
// compiler-specific stuffs
//

// compiler type
#if defined __INTEL_COMPILER // ICC must be firstly processed, as it would mimic other compiler's flags
#    define TREECORE_COMPILER_ICC 1
#elif defined _MSC_VER
#    define TREECORE_COMPILER_MSVC 1
#elif defined __clang__
#    define TREECORE_COMPILER_CLANG 1
#elif defined __GNUC__
#    define TREECORE_COMPILER_GCC 1
#elif defined DOXYGEN
#    define TREECORE_COMPILER_DOXYGEN 1
#else
#    error "unknown compiler"
#endif

#ifndef TREECORE_COMPILER_ICC
#    define TREECORE_COMPILER_ICC 0
#endif
#ifndef TREECORE_COMPILER_MSVC
#    define TREECORE_COMPILER_MSVC 0
#endif
#ifndef TREECORE_COMPILER_CLANG
#    define TREECORE_COMPILER_CLANG 0
#endif
#ifndef TREECORE_COMPILER_GCC
#    define TREECORE_COMPILER_GCC 0
#endif
#ifndef TREECORE_COMPILER_DOXYGEN
#    define TREECORE_COMPILER_DOXYGEN 0
#endif

// how to access compiler attribute
// some compilers (such as ICC) has multiple sets of builtin convensions on different platforms
#if TREECORE_COMPILER_MSVC || (TREECORE_OS_WINDOWS && TREECORE_COMPILER_ICC)
#    define TREECORE_COMPILER_ATTR_MSVC 1
#elif TREECORE_COMPILER_CLANG
#    define TREECORE_COMPILER_ATTR_CLANG 1
#elif TREECORE_COMPILER_GCC || ( ( TREECORE_OS_LINUX || TREECORE_OS_OSX) && TREECORE_COMPILER_ICC )
#    define TREECORE_COMPILER_ATTR_GCC 1
#else
#    error "don't know how to treat with compiler-specific attributes"
#endif

#ifndef TREECORE_COMPILER_ATTR_MSVC
#    define TREECORE_COMPILER_ATTR_MSVC 0
#endif
#ifndef TREECORE_COMPILER_ATTR_CLANG
#    define TREECORE_COMPILER_ATTR_CLANG 0
#endif
#ifndef TREECORE_COMPILER_ATTR_GCC
#    define TREECORE_COMPILER_ATTR_GCC 0
#endif

//
// compiler-specific stuffs
//
#if TREECORE_COMPILER_MSVC

#    if _MSC_VER >= 1900
#        define TREECORE_COMPILER_VS2015 1
#    else
#        error _MSC_VER compiler older than 1900 (Visual Studio 2015) is not supported!
#    endif

#    if TREECORE_SIZE_PTR == 8
#        define TREECORE_MSVC_INTRIN 1
#    endif

#    ifndef _CPPRTTI
#        error RTTI not enabled!
#    endif
#    ifndef _CPPUNWIND
#        error exception not enabled!
#    endif

#    pragma comment (lib, "kernel32.lib")
#    pragma comment (lib, "user32.lib")
#    pragma comment (lib, "wininet.lib")
#    pragma comment (lib, "advapi32.lib")
#    pragma comment (lib, "ws2_32.lib")
#    pragma comment (lib, "version.lib")
#    pragma comment (lib, "shlwapi.lib")
#    pragma comment (lib, "winmm.lib")

#    ifdef _NATIVE_WCHAR_T_DEFINED
#        if TREECORE_DEBUG
#            pragma comment (lib, "comsuppwd.lib")
#        else
#            pragma comment (lib, "comsuppw.lib")
#        endif
#    else
#        if TREECORE_DEBUG
#            pragma comment (lib, "comsuppd.lib")
#        else
#            pragma comment (lib, "comsupp.lib")
#        endif
#    endif
#endif

#if TREECORE_OS_WINDOWS
#    define TREECORE_LOAD_WINAPI_FUNCTION( dll, functionName, localFunctionName, returnType, params ) \
    typedef returnType ( WINAPI* type ## localFunctionName ) params;                                  \
    type ## localFunctionName localFunctionName = (type ## localFunctionName)dll.getFunction(#functionName );
#endif // TREECORE_COMPILER_MSVC

#ifndef TREECORE_MSVC_INTRIN
#    define TREECORE_MSVC_INTRIN 0
#endif

#if TREECORE_MSVC_INTRIN
#    include <intrin.h>
#endif

// calling convention
#if TREECORE_CPU_X86 && TREECORE_SIZE_PTR <= 4
#    if TREECORE_COMPILER_ATTR_MSVC
#        define TREECORE_STDCALL  __stdcall
#        define TREECORE_CDECL    __cdecl
#    else
#        define TREECORE_STDCALL  __attribute__( (stdcall) )
#        define TREECORE_CDECL    __attribute__( (cdecl) )
#    endif
#else
#    define TREECORE_STDCALL
#    define TREECORE_CDECL
#endif

#if TREECORE_COMPILER_ATTR_CLANG || TREECORE_COMPILER_ATTR_GCC
#    define TREECORE_NORETURN __attribute__ ( (noreturn) )
#elif TREECORE_COMPILER_ATTR_MSVC
#    define TREECORE_NORETURN __declspec( noreturn )
#else
#    define TREECORE_NORETURN
#endif

// API deprecation
#if TREECORE_COMPILER_DOXYGEN

///
/// \brief wrap a function which has been deprecated
///
/// synopsis: `TREECORE_DEPRECATED_FUNCTION( int foobar(int arg1, float arg2) );`
///
#    define TREECORE_DEPRECATED_FUNCTION( ... )

#elif TREECORE_NO_DEPRECATION_WARNINGS
#    define TREECORE_DEPRECATED_FUNCTION( ... ) __VA_ARGS__
#else
#    if TREECORE_COMPILER_ATTR_MSVC
#        define TREECORE_DEPRECATED_FUNCTION( ... ) __declspec( deprecated ) __VA_ARGS__
#    elif TREECORE_COMPILER_ATTR_GCC || TREECORE_COMPILER_ATTR_CLANG
#        define TREECORE_DEPRECATED_FUNCTION( ... ) __VA_ARGS__ __attribute__( (deprecated) )
#    else
#        error "don't know how to mark API deprecation in such compiler"
#    endif
#endif

// dynamic library symbol visibility
#ifndef TREECORE_DLL_BUILD
#    define TREECORE_DLL_BUILD 0
#endif
#ifndef TREECORE_USING_DLL
#    define TREECORE_USING_DLL 0
#endif

#if TREECORE_OS_WINDOWS
#    if TREECORE_COMPILER_ATTR_MSVC
#        if TREECORE_DLL_BUILD
#            define TREECORE_SHARED_API __declspec( dllexport )
#        elif TREECORE_USING_DLL
#            define TREECORE_SHARED_API __declspec( dllimport )
#        else
#            define TREECORE_SHARED_API
#        endif
#    else
#        if TREECORE_DLL_BUILD
#            define TREECORE_SHARED_API __attribute__( (dllexport) )
#        elif TREECORE_USING_DLL
#            define TREECORE_SHARED_API __attribute__( (dllimport) )
#        else
#            define TREECORE_SHARED_API
#        endif
#    endif
#    define TREECORE_SHARED_HIDDEN
#else
#    define TREECORE_SHARED_API __attribute__ ( ( visibility( "default" ) ) )
#    define TREECORE_SHARED_HIDDEN __attribute__ ( ( visibility( "hidden" ) ) )
#endif

#if TREECORE_DLL_BUILD
#    define TREECORE_PUBLIC_IN_DLL_BUILD( declaration )  public: declaration; private:
#else
#    define TREECORE_PUBLIC_IN_DLL_BUILD( declaration )  declaration;
#endif

/** This macro is added to all library API public function declarations. */
#define TREECORE_PUBLIC_FUNCTION TREECORE_SHARED_API TREECORE_STDCALL

// force inline
#if TREECORE_DEBUG || TREECORE_COMPILER_DOXYGEN
///
/// \brief platform-independent way of forcing an inline function
///
/// Use the syntax: `forcedinline void myfunction (int x)`
///
#    define forcedinline  inline
#else
#    if TREECORE_COMPILER_ATTR_MSVC
#        define forcedinline       __forceinline
#    else
#        define forcedinline       inline __attribute__( (always_inline) )
#    endif
#endif

// branch prediction hints
#if TREECORE_COMPILER_GCC
#    define likely( arg )   ( __builtin_expect( !!(arg), 1 ) )
#    define unlikely( arg ) ( __builtin_expect( !!(arg), 0 ) )
#else
#    define likely( arg ) (arg)
#    define unlikely( arg ) (arg)
#endif

// how to link identical symbol
#if TREECORE_OS_WINDOWS
#    if TREECORE_COMPILER_ATTR_MSVC
#        define TREECORE_SELECT_ANY( _arg_ ) __declspec( selectany ) _arg_
#    elif TREECORE_COMPILER_ATTR_GCC
#        define TREECORE_SELECT_ANY( _arg_ ) _arg_ __attribute__( (selectany) )
#    else
#        define TREECORE_SELECT_ANY( _arg_ ) _arg_
#    endif
#else
#    define TREECORE_SELECT_ANY( _arg_ ) _arg_
#endif

// fuck microsoft
#if TREECORE_OS_WINDOWS
#    include <Windows.h>
#    undef min
#    undef max
#endif

#endif   // TREECORE_PLATFORM_DEFS_H
