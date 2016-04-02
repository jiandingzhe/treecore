//!********************************************************************************
//! This file is part of the Three-Body Technology code base.
//! Copyright(C), 2012-2015, Three-Body Technology, all rights reserved. 
//! 
//! This file is protected by the copyright law of the people's Republic of China.
//! Permission to use this file is only granted for the employee of Three-Body 
//! Technology, or other company/group/individual who has expressly authorized by
//! Three-Body Technology. 
//!
//! @file    inline.h
//! @author  Meng Ke
//! @version 1.0
//! @date    2016/03/27
//! @brief   
//!********************************************************************************

#ifndef ____INLINE__EC95C21C_42B8_45EB_A450_E22D6160DABA
#define ____INLINE__EC95C21C_42B8_45EB_A450_E22D6160DABA

#include "../DebugFlags.h"

/*******************************************************
forcedinline and neverinline
********************************************************/
#if TREECORE_DEBUG || TREECORE_COMPILER_DOXYGEN
#   define forcedinline inline
#   define dontinline // "noinline" macro may defined by other lib, so define "dontinline" here.
#else
#   if TREECORE_COMPILER_MSVC
#       define forcedinline __forceinline
#       define dontinline __attribute__((noinline))
#   else
#       define forcedinline inline __attribute__((always_inline))
#       define dontinline __declspec(noinline)
#   endif
#endif

#endif // ____INLINE__EC95C21C_42B8_45EB_A450_E22D6160DABA