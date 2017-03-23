#ifndef TREECORE_DEBUG_UTILS_H
#define TREECORE_DEBUG_UTILS_H

#include "treecore/PlatformDefs.h"

#if TREECORE_OS_WINDOWS
#include <WinBase.h>
#endif

// break debugger
#if TREECORE_COMPILER_ATTR_MSVC
#    define TREECORE_BREAK_DEBUGGER DebugBreak()
#elif TREECORE_COMPILER_ATTR_GCC || TREECORE_COMPILER_ATTR_CLANG
#    if TREECORE_NO_INLINE_ASM
#        define TREECORE_BREAK_DEBUGGER       {}
#    else
#        define TREECORE_BREAK_DEBUGGER       { asm ("int $3"); }
#    endif
#else
#    define TREECORE_BREAK_DEBUGGER        { __asm int 3 }
#endif

// log current file location
#define TREECORE_LOG_FILE_LOCATION treecore::logAssertion( __FILE__, __LINE__ )

// debug assertion
#if TREECORE_DEBUG || TREECORE_COMPILER_DOXYGEN
/// \brief Writes a string to the standard error stream.
///
/// This is only compiled in a debug build.
///
/// \see Logger::outputDebugString
///
#    define TREECORE_DBG( dbgtext )              { treecore::String tempDbgBuf; tempDbgBuf << dbgtext; treecore::Logger::outputDebugString( tempDbgBuf ); }

/// \brief This will always cause an assertion failure.
///
/// It is only compiled in a debug build, (unless TREECORE_LOG_ASSERTIONS is enabled for your build).
///
/// \see treecore_assert
///
#    define treecore_assert_false              { TREECORE_LOG_FILE_LOCATION; if ( treecore::isRunningUnderDebugger() ) TREECORE_BREAK_DEBUGGER; }

/// Platform-independent assertion macro.
///
///   This macro gets turned into a no-op when you're building with debugging turned off, so be
///   careful that the expression you pass to it doesn't perform any actions that are vital for the
///   correct behaviour of your program!
///   @see treecore_assert_false
///
#    define treecore_assert( expression )       { if ( !(expression) ) treecore_assert_false; }

#    define TREECORE_DBGCODE( ... ) __VA_ARGS__

#else
#    define TREECORE_DBG( dbgtext )
#    define treecore_assert_false { TREECORE_LOG_FILE_LOCATION; }

#    if TREECORE_LOG_ASSERTIONS
#        define treecore_assert( expression )      { if ( !(expression) ) treecore_assert_false; }
#        define TREECORE_DBGCODE( ... ) __VA_ARGS__
#    else
#        define treecore_assert( a )               {}
#        define TREECORE_DBGCODE( ... )
#    endif
#endif

namespace treecore
{
extern TREECORE_SHARED_API bool TREECORE_STDCALL isRunningUnderDebugger();
extern TREECORE_SHARED_API void TREECORE_STDCALL logAssertion( const char* file, int line ) noexcept;
}

#endif // TREECORE_DEBUG_UTILS_H
