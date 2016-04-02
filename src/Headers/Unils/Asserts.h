//!********************************************************************************
//! This file is part of the Three-Body Technology code base.
//! Copyright(C), 2012-2015, Three-Body Technology, all rights reserved. 
//! 
//! This file is protected by the copyright law of the people's Republic of China.
//! Permission to use this file is only granted for the employee of Three-Body 
//! Technology, or other company/group/individual who has expressly authorized by
//! Three-Body Technology. 
//!
//! @file    Asserts.h
//! @author  Meng Ke
//! @version 1.0
//! @date    2016/04/03
//! @brief   
//!********************************************************************************


#ifndef ____ASSERTS__81B43ADE_BB23_4732_8FF8_B0BCFB4151DC
#define ____ASSERTS__81B43ADE_BB23_4732_8FF8_B0BCFB4151DC

#include "Complier.h"
#include "TargetAndCPU.h"
#include "DebugFlags.h"
#include "DepercatedFlags.h"

/***************************************************************
This will try to break into the debugger if the app is currently being debugged.
If called by an app that's not being debugged, the behaiour isn't defined - it may crash or not, depending
on the platform.
***************************************************************/
#if TREECORE_CPU_ARM
#    define treecore_debugBreak { ::kill (0, SIGTRAP); }
#elif TREECORE_COMPILER_MSVC
#   ifndef TREECORE_COMPILER_ICC
#       pragma intrinsic (__debugbreak)
#   endif
#   define treecore_debugBreak { __debugbreak(); }
#else
#   define treecore_debugBreak{ asm ("int $3"); }
#endif

#if JUCE_LOG_ASSERTIONS || JUCE_DEBUG
#   define TREECORE_LogCurrentAssertion treecore::logAssertion (__FILE__, __LINE__);
#else
#   define TREECORE_LogCurrentAssertion
#endif

#define tassertfalse TREECORE_MACRO_FORCED_SEMICOLON( { TREECORE_LogCurrentAssertion; treecore_debugBreak; TREECORE_TELL_STATIC_ANALYZER_NORETURN } ) 

#if TREECORE_DEBUG
#   define tassert(expression) TREECORE_MACRO_FORCED_SEMICOLON( if(!(expression)) tassertfalse; )
#   define run_with_check(experssion) tassert(experssion)
#   define TREECORE_DBGCODE(...) __VA_ARGS__
#else
#   define tassert(expression)
#   define run_with_check(experssion) experssion
#   define TREECORE_DBGCODE(...)
#endif



#endif // ____ASSERTS__81B43ADE_BB23_4732_8FF8_B0BCFB4151DC