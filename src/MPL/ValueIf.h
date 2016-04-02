//!********************************************************************************
//! This file is part of the Three-Body Technology code base.
//! Copyright(C), 2012-2015, Three-Body Technology, all rights reserved. 
//! 
//! This file is protected by the copyright law of the people's Republic of China.
//! Permission to use this file is only granted for the employee of Three-Body 
//! Technology, or other company/group/individual who has expressly authorized by
//! Three-Body Technology. 
//!
//! @file    ValueIf.h
//! @author  Meng Ke
//! @version 1.0
//! @date    2016/04/03
//! @brief   
//!********************************************************************************


#ifndef ____VALUEIF__43167B96_D850_490C_A380_ABD9900743DD
#define ____VALUEIF__43167B96_D850_490C_A380_ABD9900743DD


namespace treecore {


template< bool useFirstType , typename T1 , typename T2 >
struct TypeIf
{
    typedef T1 type;
};

template< typename T1 , typename T2 >
struct TypeIf<false , T1 , T2>
{


} //namespace treecore

#endif // ____VALUEIF__43167B96_D850_490C_A380_ABD9900743DD