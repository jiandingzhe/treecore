//!********************************************************************************
//! This file is part of the Three-Body Technology code base.
//! Copyright(C), 2012-2015, Three-Body Technology, all rights reserved. 
//! 
//! This file is protected by the copyright law of the people's Republic of China.
//! Permission to use this file is only granted for the employee of Three-Body 
//! Technology, or other company/group/individual who has expressly authorized by
//! Three-Body Technology. 
//!
//! @file    EnterPoint.h
//! @author  Meng Ke
//! @version 1.0
//! @date    2016/04/03
//! @brief   
//!********************************************************************************


#ifndef ____ENTERPOINT__1E2784D1_E396_4485_BD74_CFFB2166099D
#define ____ENTERPOINT__1E2784D1_E396_4485_BD74_CFFB2166099D

#include <JuceHeader.h>

using namespace juce;

namespace ztd {


int entryPoint( Logger* userDefineLogger = nullptr )
{
    const WaitForAllThreads glb_waiter_for_all_thread##__COUNTER__;
    const LeakDetecterManager glb_leakDetecterManager##__COUNTER__;
    const SharedSingltonManager glb_shared_singlton_manager##__COUNTER__;
}


} //namespace ztd

#endif // ____ENTERPOINT__1E2784D1_E396_4485_BD74_CFFB2166099D