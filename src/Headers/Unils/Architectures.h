//!********************************************************************************
//! This file is part of the Three-Body Technology code base.
//! Copyright(C), 2012-2015, Three-Body Technology, all rights reserved. 
//! 
//! This file is protected by the copyright law of the people's Republic of China.
//! Permission to use this file is only granted for the employee of Three-Body 
//! Technology, or other company/group/individual who has expressly authorized by
//! Three-Body Technology. 
//!
//! @file    TargetAndCPU.h
//! @author  Meng Ke
//! @version 1.0
//! @date    2016/03/27
//! @brief   
//!********************************************************************************

#ifndef ____ARCHANDCPU__6885B4BA_7871_43AC_95B0_FE0136651082
#define ____ARCHANDCPU__6885B4BA_7871_43AC_95B0_FE0136651082

#include "OS.h"

/**************************************************
下面是对CPU的选择:
TREE_CPU_INTEL
TREE_CPU_ARM
**************************************************/
#if defined(__arm__) || defined(__arm) || defined(__arm64__) || defined (__arm64__) || defined( _M_ARM ) || defined(_M_ARMT)
#   define TREE_CPU_ARM 1
#elif defined (__ppc__) || defined (__ppc64__) || defined(__powerpc) || defined(_ARCH_PPC)
#   error ppc platform is beyond the design.
#else
#   define TREE_CPU_INTEL 1
#endif

#ifndef TREE_CPU_ARM
#   define TREE_CPU_ARM 0
#endif
#ifndef TREE_CPU_INTEL
#   define TREE_CPU_INTEL 0
#endif

/**************************************************
下面是对32位和64位的选择
**************************************************/
#if defined(_WIN64)      ||\
    defined(__MINGW64__) ||\
    defined(__amd64__)   ||\
    defined(__LLP64__)   ||\
    defined(__LP64__)    ||\
    defined(_LP64)       ||\
    defined(__arm64__)   ||\
    defined(__ARM_ARCH_ISA_A64)
#   define TREE_ARCH_64BIT 1
#else
#   define TREE_ARCH_32BIT 1
#endif

#if TREE_CPU_ARM && TREE_OS_WINDOWS
#   error there is no arm processor support plan on windows yet.
#elif TREE_CPU_ARM && TREE_OS_MACOSX
#   error something wrong, I don't know there is any version of Mac OSX running on ARM.
#elif TREE_CPU_INTEL && TREE_OS_IOS
#   error something wrong, I don't know there is any version of iOS running on x86.
#endif

#ifndef TREE_ARCH_64BIT
#   define TREE_ARCH_64BIT 0
#endif
#ifndef TREE_ARCH_32BIT
#   define TREE_ARCH_32BIT 0
#endif

#endif // ____ARCHANDCPU__6885B4BA_7871_43AC_95B0_FE0136651082