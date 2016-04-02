//!********************************************************************************
//! This file is part of the Three-Body Technology code base.
//! Copyright(C), 2012-2015, Three-Body Technology, all rights reserved. 
//! 
//! This file is protected by the copyright law of the people's Republic of China.
//! Permission to use this file is only granted for the employee of Three-Body 
//! Technology, or other company/group/individual who has expressly authorized by
//! Three-Body Technology. 
//!
//! @file    custom_compiler_messages.h
//! @author  Meng Ke
//! @version 1.0
//! @date    2016/03/27
//! @brief   
//!********************************************************************************

#ifndef ____CUSTOM_COMPILER_MESSAGES__96606836_D1B6_47EC_B3B9_E110392E3EA8
#define ____CUSTOM_COMPILER_MESSAGES__96606836_D1B6_47EC_B3B9_E110392E3EA8

#include "../Complier.h"

/***************************************************************
可使用这个宏在build的时候放出一条信息
***************************************************************/
#if TREECORE_COMPILER_MSVC && !TREECORE_COMPILER_DOXYGEN
#   define ____TREECORE_CUSTOM_COMPILER_MESSAGE(file, line, mess) message( file "(" TREECPRE_MAKE_STRING(line) ") : warning: " TREECPRE_MAKE_STRING(mess) )
#   define TREECORE_CUSTOM_COMPILER_MESSAGE(message) __pragma(____TREECORE_CUSTOM_COMPILER_MESSAGE(__FILE__, __LINE__, message));
#   undef ____TREECORE_CUSTOM_COMPILER_MESSAGE
#else
#   if !defined(TREECORE_COMPILER)
#       define ____TREECORE_CUSTOM_COMPILER_MESSAGE(mess) message(TREECPRE_MAKE_STRING(mess))
#   endif
#   define TREECORE_CUSTOM_COMPILER_MESSAGE(message) _Pragma(TREECPRE_MAKE_STRING(____TREECORE_CUSTOM_COMPILER_MESSAGE(message)));
#   undef ____TREECORE_CUSTOM_COMPILER_MESSAGE
#endif

#endif // ____CUSTOM_COMPILER_MESSAGES__96606836_D1B6_47EC_B3B9_E110392E3EA8