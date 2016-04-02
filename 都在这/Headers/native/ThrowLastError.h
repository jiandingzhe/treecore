//!********************************************************************************
//! This file is part of the Three-Body Technology code base.
//! Copyright(C), 2012-2015, Three-Body Technology, all rights reserved. 
//! 
//! This file is protected by the copyright law of the people's Republic of China.
//! Permission to use this file is only granted for the employee of Three-Body 
//! Technology, or other company/group/individual who has expressly authorized by
//! Three-Body Technology. 
//!
//! @file    ThrowLastError.h
//! @author  Meng Ke
//! @version 1.0
//! @date    2016/03/19
//! @brief   
//!********************************************************************************


#ifndef ____THROWLASTERROR__1CE2F40F_D378_4F97_BDB7_5058BCDC00D4
#define ____THROWLASTERROR__1CE2F40F_D378_4F97_BDB7_5058BCDC00D4

#include <JuceHeader.h>

using namespace juce;

namespace ztd {


/// @brief  返回最后的Error信息到返回值中,如果没有错误,返回空
/// @return 一个承载着错误信息的字符串,若为空说明没有错误发生
String getLastErrorMessage();

/// @brief 如果有错误出现,调用这个函数可以抛出getLastErrorMessage
///        所返回的值.若没有错误出现则什么都不做.
void tryThrowWindowsLastError();


} //namespace ztd

#endif // ____THROWLASTERROR__1CE2F40F_D378_4F97_BDB7_5058BCDC00D4