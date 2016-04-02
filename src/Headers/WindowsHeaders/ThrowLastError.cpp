//!********************************************************************************
//! This file is part of the Three-Body Technology code base.
//! Copyright(C), 2012-2015, Three-Body Technology, all rights reserved. 
//! 
//! This file is protected by the copyright law of the people's Republic of China.
//! Permission to use this file is only granted for the employee of Three-Body 
//! Technology, or other company/group/individual who has expressly authorized by
//! Three-Body Technology. 
//!
//! @file    ThrowLastError.cpp
//! @author  Meng Ke
//! @version 1.0
//! @date    2016/03/19
//! @brief   
//!********************************************************************************


#include "minimal_windows.h"
#include "ThrowLastError.h"
#include "Details/Exception.h"


namespace ztd {


void tryThrowWindowsLastError()
{
    String errMesssage = getLastErrorMessage();
    if( errMesssage != String::empty() )
    {
        jassertfalse;
        Logger::writeToLog( errMesssage );
        ZTD_THROW_EXCEPTION( errMesssage );
    }
}


String getLastErrorMessage()
{
    String errorMessage = String::empty();
    DWORD errCode = GetLastError();
    if( errCode != ERROR_SUCCESS )
    {
        errorMessage = String( "windows secific exception :" ) + String( (int)errCode );
    }
    return errorMessage;
}


} // ztd