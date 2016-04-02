//!********************************************************************************
//! This file is part of the Three-Body Technology code base.
//! Copyright(C), 2012-2015, Three-Body Technology, all rights reserved. 
//! 
//! This file is protected by the copyright law of the people's Republic of China.
//! Permission to use this file is only granted for the employee of Three-Body 
//! Technology, or other company/group/individual who has expressly authorized by
//! Three-Body Technology. 
//!
//! @file    OS.h
//! @author  Meng Ke
//! @version 1.0
//! @date    2016/03/27
//! @brief   
//!********************************************************************************

#ifndef ____OS__F04AB564_00F6_47B1_BCE1_0F7C4E728E6D
#define ____OS__F04AB564_00F6_47B1_BCE1_0F7C4E728E6D

/**************************************************
下面是对OS的检测,treecore一共支持以下几种操作系统:
TREECORE_OS_WINDOWS
TREECORE_OS_MACOSX
TREECORE_OS_LINUX
TREECORE_OS_IOS
TREECORE_OS_ANDROID

* windows phone? 暂时没计划.
**************************************************/
#if ( defined( _WIN32 ) || defined( _WIN64 ) )
#   define TREECORE_OS_WINDOWS 1
#elif ( defined( __ANDROID__ ) || defined( ANDROID ) )
#   define TREECORE_OS_ANDROID 1
#elif ( defined( LINUX ) || defined( __linux__ ) )
#   define TREECORE_OS_LINUX 1
#elif ( defined( __APPLE__ ) || defined( __APPLE_CPP__ ) || defined( __APPLE_CC__ ) )
#   include <TargetConditionals.h> // include this header file to tell if "TARGET_OS_IPHONE" defined.
#   if TARGET_OS_IPHONE || TARGET_IPHONE_SIMULATOR
#      define TREECORE_OS_IOS 1 // there is no way to know that OS is iphone or ipad. 
#   else
#      define TREECORE_OS_MACOSX 1
#   endif
#else
#   error "Unknown platform!"
#endif

#ifndef TREECORE_OS_WINDOWS
#   define TREECORE_OS_WINDOWS 0
#endif
#ifndef TREECORE_OS_ANDROID
#   define TREECORE_OS_ANDROID 0
#endif
#ifndef TREECORE_OS_LINUX
#   define TREECORE_OS_LINUX 0
#endif
#ifndef TREECORE_OS_IOS
#   define TREECORE_OS_IOS 0
#endif
#ifndef TREECORE_OS_MACOSX
#   define TREECORE_OS_MACOSX 0
#endif

#endif // ____OS__F04AB564_00F6_47B1_BCE1_0F7C4E728E6D