//!********************************************************************************
//! This file is part of the Three-Body Technology code base.
//! Copyright(C), 2012-2015, Three-Body Technology, all rights reserved. 
//! 
//! This file is protected by the copyright law of the people's Republic of China.
//! Permission to use this file is only granted for the employee of Three-Body 
//! Technology, or other company/group/individual who has expressly authorized by
//! Three-Body Technology. 
//!
//! @file    end_mpl_function.h
//! @author  Meng Ke
//! @version 1.0
//! @date    2016/02/26
//! @brief   
//!********************************************************************************


#define TYPE_FUNCTION(result,funName,args,...)\
template< args >\
struct funName

#define RETURN_TYPE_BY_NAME(type,...) typedef typename __VA_ARGS__ type;