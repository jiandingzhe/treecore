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

#include "treecore/SystemStats.h"

#include "treecore/HeapBlock.h"
#include "treecore/Logger.h"
#include "treecore/NewLine.h"

#include "treecore/internal/SystemStats_private.h"

#if TREECORE_OS_WINDOWS
#    include <DbgHelp.h>
#else
#    include "treecore/native/posix_private.h"
#    include <execinfo.h>
#    include <signal.h>
#endif

namespace treecore {

//==============================================================================

static const CPUInformation& getCPUInformation() noexcept
{
    static CPUInformation info;
    return info;
}

int SystemStats::getNumCpus() noexcept        { return getCPUInformation().numCpus; }
bool SystemStats::hasMMX() noexcept           { return getCPUInformation().hasMMX; }
bool SystemStats::hasSSE() noexcept           { return getCPUInformation().hasSSE; }
bool SystemStats::hasSSE2() noexcept          { return getCPUInformation().hasSSE2; }
bool SystemStats::hasSSE3() noexcept          { return getCPUInformation().hasSSE3; }
bool SystemStats::has3DNow() noexcept         { return getCPUInformation().has3DNow; }

//==============================================================================
String SystemStats::getStackBacktrace()
{
    String result;

   #if TREECORE_OS_ANDROID || (TREECORE_OS_WINDOWS && TREECORE_COMPILER_GCC)
    treecore_assert_false; // sorry, not implemented yet!

   #elif TREECORE_OS_WINDOWS
    HANDLE process = GetCurrentProcess();
    SymInitialize( process, nullptr, TRUE );

    void* stack[128];
    int frames = (int) CaptureStackBackTrace( 0, numElementsInArray( stack ), stack, nullptr );

    HeapBlock<SYMBOL_INFO> symbol;
    symbol.calloc( sizeof(SYMBOL_INFO) + 256, 1 );
    symbol->MaxNameLen   = 255;
    symbol->SizeOfStruct = sizeof(SYMBOL_INFO);

    for (int i = 0; i < frames; ++i)
    {
        DWORD64 displacement = 0;

        if ( SymFromAddr( process, (DWORD64) stack[i], &displacement, symbol ) )
        {
            result << i << ": ";

            IMAGEHLP_MODULE64 moduleInfo;
            zerostruct( moduleInfo );
            moduleInfo.SizeOfStruct = sizeof(moduleInfo);

            if ( ::SymGetModuleInfo64( process, symbol->ModBase, &moduleInfo ) )
                result << moduleInfo.ModuleName << ": ";

            result << symbol->Name << " + 0x" << String::toHexString( (int64) displacement ) << newLine;
        }
    }

   #else
    void* stack[128];
    int frames = backtrace( stack, numElementsInArray( stack ) );
    char** frameStrings = backtrace_symbols( stack, frames );

    for (int i = 0; i < frames; ++i)
        result << frameStrings[i] << newLine;

    ::free( frameStrings );
   #endif

    return result;
}

//==============================================================================
SystemStats::CrashHandlerFunction globalCrashHandler = nullptr;

void SystemStats::setApplicationCrashHandler( CrashHandlerFunction handler )
{
    treecore_assert( handler != nullptr ); // This must be a valid function.
    globalCrashHandler = handler;

#if TREECORE_OS_WINDOWS
    SetUnhandledExceptionFilter( handleCrash );
#else
    const int signals[] = { SIGFPE, SIGILL, SIGSEGV, SIGBUS, SIGABRT, SIGSYS };

    for (int i = 0; i < numElementsInArray( signals ); ++i)
    {
        ::signal( signals[i], handleCrash );
        _sig_interrupt_( signals[i], 1 );
    }
#endif
}

}
