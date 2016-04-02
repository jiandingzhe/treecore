//!********************************************************************************
//! This file is part of the Three-Body Technology code base.
//! Copyright(C), 2012-2015, Three-Body Technology, all rights reserved. 
//! 
//! This file is protected by the copyright law of the people's Republic of China.
//! Permission to use this file is only granted for the employee of Three-Body 
//! Technology, or other company/group/individual who has expressly authorized by
//! Three-Body Technology. 
//!
//! @file    WaitForAllThreads.h
//! @author  Meng Ke
//! @version 1.0
//! @date    2016/04/03
//! @brief   
//!********************************************************************************


#ifndef ____WAITFORALLTHREADS__76AD1C90_6D6F_41C6_A565_94555371D8E3
#define ____WAITFORALLTHREADS__76AD1C90_6D6F_41C6_A565_94555371D8E3

#include <JuceHeader.h>

using namespace juce;

namespace ztd {


class WaitForAllThreads
{
public:
    WaitForAllThreads() = default;
    ~WaitForAllThreads()
    {
        for( int i = 0;; ++i )
        {
            int k = getThreadCounter().get();
            if( k == 0 ) return;
            tassert( k >= 0 );
            if( i % 64 == 0 ) Thread::sleep( 20 );
        }
    }
private:
	
    static Atomic<int>& getThreadCounter()
    {
        static Atomic<int> counter;
        return counter;
    }

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(WaitForAllThreads);
};


} //namespace ztd

#endif // ____WAITFORALLTHREADS__76AD1C90_6D6F_41C6_A565_94555371D8E3