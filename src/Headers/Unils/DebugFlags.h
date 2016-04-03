//!********************************************************************************
//! This file is part of the Three-Body Technology code base.
//! Copyright(C), 2012-2015, Three-Body Technology, all rights reserved. 
//! 
//! This file is protected by the copyright law of the people's Republic of China.
//! Permission to use this file is only granted for the employee of Three-Body 
//! Technology, or other company/group/individual who has expressly authorized by
//! Three-Body Technology. 
//!
//! @file    DebugFlags.h
//! @author  Meng Ke
//! @version 1.0
//! @date    2016/03/27
//! @brief   
//!********************************************************************************


#ifndef ____DEBUGFLAGS__17D3A5E0_673E_45DB_8188_5B882E126A49
#define ____DEBUGFLAGS__17D3A5E0_673E_45DB_8188_5B882E126A49

/***************************************************
下面我们检查是debug还是release
treecore在debug模式和release模式下很多行为会有所不同,
其中有很多会影响效率,因此务必确保debug和release的正确设置.
如果想在release模式下依然开启一些断言检查,可在项目设置中
定义"TREE_FORCE_DEBUG"宏.
***************************************************/
#if defined(__DEBUG) || defined(_DEBUG) || defined(DEBUG) || defined(__DEBUG__) || TREE_FORCE_DEBUG
#   define TREE_DEBUG 1
#endif
#if !TREE_FORCE_DEBUG && ( defined(__NDEBUG) || defined(_NDEBUG) || defined(NDEBUG) || defined(__NDEBUG__) )
#   define TREE_RELEASE 1
#endif

#ifndef TREE_FORCE_DEBUG
#   define TREE_FORCE_DEBUG 0
#endif
#ifndef TREE_RELEASE
#   define TREE_RELEASE 0
#endif

#if TREE_DEBUG && TREE_RELEASE
#   error both "release" and "debug" flags defined! can not know witch to use.
#endif
#if !TREE_DEBUG && !TREE_RELEASE
#   error both "release" and "debug" flags are NOT defined! can not know witch to use.
#endif

#endif // ____DEBUGFLAGS__17D3A5E0_673E_45DB_8188_5B882E126A49