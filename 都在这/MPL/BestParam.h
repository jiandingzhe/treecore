//!********************************************************************************
//! This file is part of the Three-Body Technology code base.
//! Copyright(C), 2012-2015, Three-Body Technology, all rights reserved. 
//! 
//! This file is protected by the copyright law of the people's Republic of China.
//! Permission to use this file is only granted for the employee of Three-Body 
//! Technology, or other company/group/individual who has expressly authorized by
//! Three-Body Technology. 
//!
//! @file    BestParam.h
//! @author  Meng Ke
//! @version 1.0
//! @date    2016/02/27
//! @brief   
//!********************************************************************************

#ifndef ____BESTPARAM__12C4C41E_93A0_45C7_8656_F9D6F9262E4F
#define ____BESTPARAM__12C4C41E_93A0_45C7_8656_F9D6F9262E4F

#include "TypeIf.h"

namespace treecore {

///! 这个模板以最"合适"的方式传递参数:
///! @code
///! template< typename T >
///! void foo( T bar ); //这里该传值? 传引用?
///! @endcode
///! 一些轻量级的类型传引用比较浪费,传值合适,而另一些
///! 大一点的类,或者无法拷贝的类只能传引用.这就导致了
///! 模板函数的参数的难以选择.
///! 使用BestParam<T>::type则可以以最"合适"的方式传参.
///! 使用方法如下:
///! @code
///! template< typename T >
///! void foo( BestParam<T>::type bar ); //自动选择值类型或者引用类型
///! @endcode 
///! 所谓"最合适"的方式如下:
///! + 算数类型(int,float,double...)->值传递
///! + 指针类型->值传递
///! + 可平凡拷贝的类型->大于4或8(取决于32位还是64位),const引用传递,否则值传递.
///! + 不可拷贝的类型->const引用传递
///! + 可拷贝但拷贝不平凡的->const引用传递
///! + 数组类型->使用指针传递
///! @warning 多说一句,万一有人问"我非要可修改的引用传递怎么办?" 直接写呗......
template <typename T>
struct BestParam
{
    typedef typename TypeIf<
        std::is_arithmetic<T>::value ||
        std::is_enum<T>::value ||
        std::is_pointer<T>::value ||
        ( std::is_trivially_copyable<T>::value && sizeof(T)<=sizeof(void*) ) ,
        T
        const T&
    >::type type;
};

#define BEST_PARAM_MACRO(srcT,targetT,...)\
template <typename T ,##__VA_ARGS__ >\
struct BestParam<srcT>\
{\
    typedef targetT type;\
}

BEST_PARAM_MACRO( T& , T& );
BEST_PARAM_MACRO( const T& , const T& );
BEST_PARAM_MACRO( T&& , T&& );
BEST_PARAM_MACRO( T[N] , T* , std::size_t N );
BEST_PARAM_MACRO( const T[N] , const T* , std::size_t N );

函数指针要搞一下,值传递反而能inline

#undef BEST_PARAM_MACRO

} //namespace treecore

#endif // ____BESTPARAM__12C4C41E_93A0_45C7_8656_F9D6F9262E4F