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

#include "Headers/Unils/Complier.h"
#include "Headers/Unils/Architectures.h"
#include "Headers/Unils/DebugFlags.h"

/***************************************************************
This will try to break into the debugger if the app is currently being debugged.
If called by an app that's not being debugged, the behaiour isn't defined - it may crash or not, depending
on the platform.
***************************************************************/
#if TREE_CPU_ARM
#    define tree_debugBreak { ::kill (0, SIGTRAP); }
#elif TREE_COMPILER_MSVC
#   if !TREE_COMPILER_INTEL_ICC
#       pragma intrinsic (__debugbreak)
#   endif
#   define tree_debugBreak { __debugbreak(); }
#else //GCC
#   define tree_debugBreak { asm("int $3"); }
#endif

/// @breif 静态断言,和标准静态断言的区别是这个静态断言可以不写message
#define tstatic_assert(x,...) static_assert((x),""__VA_ARGS__)


#if JUCE_LOG_ASSERTIONS || TREE_DEBUG
#   define TREE_LogCurrentAssertion treecore::logAssertion (__FILE__, __LINE__);
#else
#   define TREE_LogCurrentAssertion
#endif

#define tassertfalse TREE_MACRO_FORCED_SEMICOLON( { TREE_LogCurrentAssertion; tree_debugBreak; TREE_TELL_STATIC_ANALYZER_NORETURN } ) 

#if TREE_DEBUG
#   define tassert(expression) TREE_MACRO_FORCED_SEMICOLON( if(!(expression)) tassertfalse; )
#   define run_with_check(experssion) tassert(experssion)
#   define TREE_DBGCODE(...) __VA_ARGS__
#else
#   define tassert(expression)
#   define run_with_check(experssion) experssion
#   define TREE_DBGCODE(...)
#endif



#endif // ____ASSERTS__81B43ADE_BB23_4732_8FF8_B0BCFB4151DC