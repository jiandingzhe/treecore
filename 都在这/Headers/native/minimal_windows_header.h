//!********************************************************************************
//! This file is part of the Three-Body Technology code base.
//! Copyright(C), 2012-2015, Three-Body Technology, all rights reserved. 
//! 
//! This file is protected by the copyright law of the people's Republic of China.
//! Permission to use this file is only granted for the employee of Three-Body 
//! Technology, or other company/group/individual who has expressly authorized by
//! Three-Body Technology. 
//!
//! @file    minimal_windows.h
//! @author  Meng Ke
//! @version 1.0
//! @date    2016/03/19
//! @brief   
//!********************************************************************************


#ifndef ____MINIMAL_WINDOWS__FCB4B14D_1B86_4BED_B527_523060EEB1D2
#define ____MINIMAL_WINDOWS__FCB4B14D_1B86_4BED_B527_523060EEB1D2


/*

this file is a minimal include <windows.h> since windows.h has so many 
marco defines and maybe make chaos.

*/

//* without this marco, windows.h will use ANSI version of functions other 
//  than UTF-16. and we don't want this happen.
#ifndef UNICODE
#define UNICODE
#endif

//* this will minimal the unused codes in windows.h
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

//* if u don't declare this marco before include windows.h, 
//  u will get sooo many wired things like getting error on std::min,std::max,
//  that's because "min" and "max" have already declared as a marco in windows.h !!
#ifndef NOMINMAX 
#define NOMINMAX
#endif

#include <windows.h>

#endif // ____MINIMAL_WINDOWS__FCB4B14D_1B86_4BED_B527_523060EEB1D2