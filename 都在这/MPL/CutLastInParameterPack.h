//!********************************************************************************
//! This file is part of the Three-Body Technology code base.
//! Copyright(C), 2012-2015, Three-Body Technology, all rights reserved. 
//! 
//! This file is protected by the copyright law of the people's Republic of China.
//! Permission to use this file is only granted for the employee of Three-Body 
//! Technology, or other company/group/individual who has expressly authorized by
//! Three-Body Technology. 
//!
//! @file    CutLastInParameterPack.h
//! @author  Meng Ke
//! @version 1.0
//! @date    2016/02/25
//! @brief   
//!********************************************************************************

#ifndef ____CUTLASTINPARAMETERPACK__B6AA9AAF_39F5_41DA_A99D_6A7F24E96A0E
#define ____CUTLASTINPARAMETERPACK__B6AA9AAF_39F5_41DA_A99D_6A7F24E96A0E

namespace treecore {
namespace details {

template <typename ...P> 
struct dummy {};

template <template <typename ...> class Obj , typename T , typename ...P> 
struct internal;

template <template <typename ...> class Obj , typename ...P1 , typename T , typename ...P2> 
struct internal<Obj , dummy<P1...> , T , P2...>
{
    using type = typename internal<Obj , dummy<P1... , T> , P2...>::type;
};

template <template <typename ...> class Obj , typename ...P1 , typename T , typename L> 
struct internal<Obj , dummy<P1...> , T , L>
{
    using type = Obj<P1... , T>;
};

template <template <typename ...> class T , typename ...P> 
struct CutLastElementInParameterPack
{
    using type = typename internal<T , dummy<> , P...>::type;
};

}
}



#endif // ____CUTLASTINPARAMETERPACK__B6AA9AAF_39F5_41DA_A99D_6A7F24E96A0E