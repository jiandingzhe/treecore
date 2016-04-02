//!********************************************************************************
//! This file is part of the Three-Body Technology code base.
//! Copyright(C), 2012-2015, Three-Body Technology, all rights reserved. 
//! 
//! This file is protected by the copyright law of the people's Republic of China.
//! Permission to use this file is only granted for the employee of Three-Body 
//! Technology, or other company/group/individual who has expressly authorized by
//! Three-Body Technology. 
//!
//! @file    Instructions.h
//! @author  Meng Ke
//! @version 1.0
//! @date    2016/03/27
//! @brief   
//!********************************************************************************

#ifndef ____INSTRUCTIONS__AE2DAE50_A9B8_4949_A4EF_79A58E9132E1
#define ____INSTRUCTIONS__AE2DAE50_A9B8_4949_A4EF_79A58E9132E1

#include "TargetAndCPU.h"
#include <xmmintrin.h>
#include <emmintrin.h>

/********************************************************

接下来我们要设置一些flag来区分一些SIMD指令集,在ARM上treecore需要
NEON指令集来进行一些手动优化,在intel上则最少需要SSE3指令集.同时,随着
用户开启不同的指令集,treecore会选择不同的指令集来进行某些代码的优化方案.
当然,选择的指令集越新,对用户的电脑CPU要求也越新.

* SSE3指令集最低在奔腾4的CPU上即支持了,一方面考虑到奔腾4已经是10年前的CPU了,
另一方面SSE3提供了很多很有用的交叉平行化指令,因此我们不打算比这个再低了.

* iphone4的处理器ARMv7已经可以支持NEON指令集了,我们看不出还有任何
在IOS上不使用NEON的理由.

* 某些Android可能使用了老的处理器而不支持NEON,在可预见的未来都没有支持
他们的计划.

* __ARM_NEON__ is legacy, since it is not defined in the
ARM(R) C Language Extensions Release 1.1:
http://infocenter.arm.com/help/topic/com.arm.doc.ihi0053b/IHI0053B_arm_c_language_extensions_2013.pdf.
Instead, __ARM_NEON should be used, see chapter 4.4 (ACLE headers).
********************************************************/
#if TREECORE_CPU_ARM
#   if defined(__ARM_NEON__) || defined(__ARM_NEON)
#       define TREECORE_INSTRUCTION_NEON 1
#   else
#       error can not build treecore on a ARM CPU without NEON.
#   endif
#else
#   if defined(__SSE__)
#       define TREECORE_INSTRUCTION_SSE1 1
#   endif
#   if defined(__SSE2__)
#       define TREECORE_INSTRUCTION_SSE2 1
#   endif
#   if defined(__SSE3__)
#       define TREECORE_INSTRUCTION_SSE3 1
#   endif
#   if defined(__SSSE3__)
#       define TREECORE_INSTRUCTION_SSSE3 1
#   endif
#   if defined(__SSE4_1__)
#       define TREECORE_INSTRUCTION_AVX41 1
#   endif
#   if defined(__SSE4_2__)
#       define TREECORE_INSTRUCTION_SSE42 1
#   endif
#   if defined(__AVX__)
#       define TREECORE_INSTRUCTION_AVX1 1
#   endif
#   if defined(__AVX2__)
#       define TREECORE_INSTRUCTION_AVX2 1
#   endif
#endif

#if !TREECORE_INSTRUCTION_SSE1 || !TREECORE_INSTRUCTION_SSE2 || !TREECORE_INSTRUCTION_SSE3
/*
if you hit this, the most reason is you need to set SSE3 flag on your complier,
like "-msse3" on GCC. most complier genrate bad asm code which is sooo slow if
no SSE flag set. treecore need at least SSE3 instruction to build and now a day
almost all CPU have SSE3.
*/
#   error treecore need at least SSE3 instruction to build.
#endif

#endif // ____INSTRUCTIONS__AE2DAE50_A9B8_4949_A4EF_79A58E9132E1