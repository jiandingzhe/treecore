//!********************************************************************************
//! This file is part of the Three-Body Technology code base.
//! Copyright(C), 2012-2015, Three-Body Technology, all rights reserved. 
//! 
//! This file is protected by the copyright law of the people's Republic of China.
//! Permission to use this file is only granted for the employee of Three-Body 
//! Technology, or other company/group/individual who has expressly authorized by
//! Three-Body Technology. 
//!
//! @file    StdIncludes.h
//! @author  Meng Ke
//! @version 1.0
//! @date    2016/04/03
//! @brief   
//!********************************************************************************


#ifndef ____STDINCLUDES__74611113_A04F_4EBA_A2AB_14C2CC310D9C
#define ____STDINCLUDES__74611113_A04F_4EBA_A2AB_14C2CC310D9C


#include <functional>
#include <tuple>
#include <type_traits>
#include <limits>
#include <bitset>
#include <new>
#include <typeindex>

namespace treecore {


using std::tuple;
using std::function;
using std::pair;
using std::bitset;

using std::type_info;
using std::hash;
using std::type_index;

using std::bad_cast;
using std::bad_typeid;

template< typename T >
using limits = std::numeric_limits<T>;


} //namespace treecore


#endif // ____STDINCLUDES__74611113_A04F_4EBA_A2AB_14C2CC310D9C