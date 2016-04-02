//!********************************************************************************
//! This file is part of the Three-Body Technology code base.
//! Copyright(C), 2012-2015, Three-Body Technology, all rights reserved. 
//! 
//! This file is protected by the copyright law of the people's Republic of China.
//! Permission to use this file is only granted for the employee of Three-Body 
//! Technology, or other company/group/individual who has expressly authorized by
//! Three-Body Technology. 
//!
//! @file    TypeIf.h
//! @author  Meng Ke
//! @version 1.0
//! @date    2016/02/25
//! @brief   
//!********************************************************************************

#ifndef ____TYPEIF__A7CA3731_0ADA_434D_A86D_4D7E696C035F
#define ____TYPEIF__A7CA3731_0ADA_434D_A86D_4D7E696C035F

template< bool useFirstType , typename T1 , typename T2 >
struct TypeIf
{
    typedef T1 type;
};

template< typename T1 , typename T2 >
struct TypeIf<false , T1 , T2>
{
    typedef T2 type;
};

#endif // ____TYPEIF__A7CA3731_0ADA_434D_A86D_4D7E696C035F