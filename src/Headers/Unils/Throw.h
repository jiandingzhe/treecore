//!********************************************************************************
//! This file is part of the Three-Body Technology code base.
//! Copyright(C), 2012-2015, Three-Body Technology, all rights reserved. 
//! 
//! This file is protected by the copyright law of the people's Republic of China.
//! Permission to use this file is only granted for the employee of Three-Body 
//! Technology, or other company/group/individual who has expressly authorized by
//! Three-Body Technology. 
//!
//! @file    Throw.h
//! @author  Meng Ke
//! @version 1.0
//! @date    2016/04/03
//! @brief   
//!********************************************************************************


#ifndef ____THROW__1BFC054E_E4FA_40DE_9EFC_06C570FBF58B
#define ____THROW__1BFC054E_E4FA_40DE_9EFC_06C570FBF58B


#define TREECORE_THROW(...) \
    tassertfalse;\
    throw __VA_ARGS__


#endif // ____THROW__1BFC054E_E4FA_40DE_9EFC_06C570FBF58B