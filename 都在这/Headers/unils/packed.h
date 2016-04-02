//!********************************************************************************
//! This file is part of the Three-Body Technology code base.
//! Copyright(C), 2012-2015, Three-Body Technology, all rights reserved. 
//! 
//! This file is protected by the copyright law of the people's Republic of China.
//! Permission to use this file is only granted for the employee of Three-Body 
//! Technology, or other company/group/individual who has expressly authorized by
//! Three-Body Technology. 
//!
//! @file    packed.h
//! @author  Meng Ke
//! @version 1.0
//! @date    2016/03/27
//! @brief   
//!********************************************************************************

#ifndef ____PACKED__4F193C88_4CFD_4567_8ADC_AED9CC047FFD
#define ____PACKED__4F193C88_4CFD_4567_8ADC_AED9CC047FFD

#include "../Complier.h"

/***************************************************************
结构体的参数对齐,用法:
@code
PACKED(
struct foo
{
int bar;
});
@endcode
***************************************************************/
#if TREECORE_COMPILER_MSVC
#   define PACKED(...) __pragma(pack(push, 1)) __VA_ARGS__ __pragma(pack(pop))
#else
#   define PACKED(...) __VA_ARGS__ __attribute__((__packed__))
#endif

#endif // ____PACKED__4F193C88_4CFD_4567_8ADC_AED9CC047FFD