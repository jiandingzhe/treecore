//!********************************************************************************
//! This file is part of the Three-Body Technology code base.
//! Copyright(C), 2012-2015, Three-Body Technology, all rights reserved. 
//! 
//! This file is protected by the copyright law of the people's Republic of China.
//! Permission to use this file is only granted for the employee of Three-Body 
//! Technology, or other company/group/individual who has expressly authorized by
//! Three-Body Technology. 
//!
//! @file    likely.h
//! @author  Meng Ke
//! @version 1.0
//! @date    2016/03/27
//! @brief   
//!********************************************************************************

#ifndef ____LIKELY__DA3C01E4_9DE2_4CEC_BBD9_3206ED49B5C0
#define ____LIKELY__DA3C01E4_9DE2_4CEC_BBD9_3206ED49B5C0

#include "../Complier.h"

#if ! TREECORE_COMPILER_MSVC
#   define likely(x) __builtin_expect(!!(x),1)
#   define likely_if(x) if(likely(x))
#   define likely_while(x) while(likely(x))
#   define unlikely(x)  __builtin_expect(!!(x),0)
#   define unlikely_if(x) if(unlikely(x))
#   define unlikely_while(x) while(unlikely(x))
#else
#   define likely(x) (x)
#   define likely_if(x) if(x)
#   define likely_while(x) while(likely(x))
#   define unlikely(x) (x)
#   define unlikely_if(x) if(x)
#   define unlikely_while(x) while(unlikely(x))
#endif

#endif // ____LIKELY__DA3C01E4_9DE2_4CEC_BBD9_3206ED49B5C0