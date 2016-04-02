//!********************************************************************************
//! This file is part of the Three-Body Technology code base.
//! Copyright(C), 2012-2015, Three-Body Technology, all rights reserved. 
//! 
//! This file is protected by the copyright law of the people's Republic of China.
//! Permission to use this file is only granted for the employee of Three-Body 
//! Technology, or other company/group/individual who has expressly authorized by
//! Three-Body Technology. 
//!
//! @file    Complier.h
//! @author  Meng Ke
//! @version 1.0
//! @date    2016/03/27
//! @brief   
//!********************************************************************************

#ifndef ____COMPLIER__572642FE_0E30_4CA6_9E47_7B28603375C4
#define ____COMPLIER__572642FE_0E30_4CA6_9E47_7B28603375C4

#include "UserFlags.h"

/**********************************************************************
查找编译器类型,TREECORE支持的编译器共有以下几种:
TREECORE_COMPILER_GCC : GCC
TREECORE_COMPILER_MSVC : 微软C++编译器
TREECORE_COMPILER_MINGW : minGW的编译器
TREECORE_COMPILER_INTEL_ICC : Intel ICC编译器
TREECORE_COMPILER_CLANG : Clang编译器
TREECORE_COMPILER_DOXYGEN : DOXYGEN的解析器

请注意这些编译器宏并非是排它的,
其中,Clang编译器,minGW的编译器和ICC编译器要依托GCC或MSVC编译器,
因此有可能同时有多个编译器宏被指定,例如,在windows平台下使用
ICC编译器,则TREECORE_COMPILER_MSVC和TREECORE_COMPILER_INTEL_ICC两个宏
会同时被定义(因为ICC在windows下依赖vc编译器),而在mac下使用clang编译器
也会导致TREECORE_COMPILER_GCC和TREECORE_COMPILER_CLANG同时被定义.

这样做的目的是,例如clang在大多数情况下行为和GCC很像,GCC支持的一些特殊特性
clang也支持,因此大部分情况下clang还是和GCC共享同样的代码,而如果真的遇到clang
的特殊代码,则直接使用#if TREECORE_COMPILER_CLANG 即可. ICC,minGW也是类似的情况.

* 值得说明的是minGW并算不上是个独立的编译器,但由于其在windows的特殊性,
它与GCC在一些东西上的表现实在是不一样,因此单独提了出来.
windows上的GCC项目目前常见的有:minGW,minGW-w64,TDM-GCC32,TDM-GCC64
这里不好意思一下,包括TDM GCC我们也叫他MINGW了.

* DOXYGEN的解析器也算不上是完整的编译器,但我们需要针对DOXYGEN进行一些特殊动作,
例如隐藏某个私有类,去掉一些不必要的标示.TREECORE_COMPILER_DOXYGEN这个宏
存在的意义主要在此.

* 除了上文中提到的编译器,其他编译器在短期内均无任何支持计划.不过,介于TREECORE
的开发者们会尽量使用标准C++,因此并不代表其他编译器不能编译TREECORE,但我们
短期内不会为除上文提到的编译器以外的任何编译器做任何测试.

************************************************************************/
#if defined( __GNUC__ )
#   define TREECORE_COMPILER_GCC 1
#elif defined( _MSC_VER )
#   define TREECORE_COMPILER_MSVC 1
#elif defined( DOXYGEN )
#   define TREECORE_COMPILER_DOXYGEN 1
#else
#   error unsupported compiler
#endif

#if defined( __MINGW32__ ) || defined( __MINGW64__ )
#   define TREECORE_COMPILER_MINGW 1
#endif

#if defined( __ICC ) || defined( __ICL ) || defined( __INTEL_COMPILER )
#   define TREECORE_COMPILER_INTEL_ICC 1
#endif

#ifdef __clang__
#   define TREECORE_COMPILER_CLANG 1
#endif

#ifndef TREECORE_COMPILER_GCC
#   define TREECORE_COMPILER_GCC 0
#endif
#ifndef TREECORE_COMPILER_MSVC
#   define TREECORE_COMPILER_MSVC 0
#endif
#ifndef TREECORE_COMPILER_DOXYGEN
#   define TREECORE_COMPILER_DOXYGEN 0
#endif
#ifndef TREECORE_COMPILER_MINGW
#   define TREECORE_COMPILER_MINGW 0
#endif
#ifndef TREECORE_COMPILER_INTEL_ICC
#   define TREECORE_COMPILER_INTEL_ICC 0
#endif
#ifndef TREECORE_COMPILER_CLANG
#   define TREECORE_COMPILER_CLANG 0
#endif


/*********************************************************************
TREECORE的编译需要编译器完整支持C++11的绝大部分特性,这大致包括:

0. override与final
1. noexcept
2. lambda
3. 可变参数类模板
4. 初始化列表
5. tuple
6. 拷贝构造=delete
7. 右值引用
8. Non-static data member initializers
9. 静态断言
10. auto type
11. Explicit构造函数
12. Range-based for
13. Extern templates
14. alignas

完整支持这些特性的编译器如下:
MSVC: VS 2015
GCC: 3.8( 3.7 已经可以了但是icc在linux下需要GCC到3.8才能支持我们需要的特性,反正不差多少,就3.8吧 )
clang: 3.1
icc: 14

* 兼容不完整的C++11会导致很多麻烦,这会浪费很多的时间,treecore刚开始开发的时候已经2015年了,
在目前的IT技术发展速度下,我们不认为还有任何不使用C++11的理由.

**********************************************************************/
#if TREECORE_COMPILER_MSVC && !TREECORE_COMPILER_INTEL_ICC
#   if _MSC_VER < 1900 //VS 2015
#       define TREECORE_COMPILER_NOT_GOOD
#   endif
#endif

#if TREECORE_COMPILER_GCC && !TREECORE_COMPILER_CLANG
#   if ( __GNUC__ * 100 + __GNUC_MINOR__ ) < 308
#       define TREECORE_COMPILER_NOT_GOOD
#   endif
#endif

#if TREECORE_COMPILER_CLANG
#   if ( __clang_major__*100 + __clang_minor__ ) < 301
#       define TREECORE_COMPILER_NOT_GOOD
#   endif
#endif

#if TREECORE_COMPILER_INTEL_ICC
#   if defined(__INTEL_COMPILER)
#       if __INTEL_COMPILER < 1400
#           define TREECORE_COMPILER_NOT_GOOD
#       endif
#   elif defined(__ICL)
#       if __ICL < 1400
#           define TREECORE_COMPILER_NOT_GOOD
#       endif
#   elif defined(__ICC)
#       if __ICC < 1400
#           define TREECORE_COMPILER_NOT_GOOD
#       endif
#   else
#       error can not know ICC version some how!
#   endif
#endif

#ifdef TREECORE_COMPILER_NOT_GOOD
#   undef TREECORE_COMPILER_NOT_GOOD
#   error TREECORE need a good C++11 compiler to build!
#endif


#endif // ____COMPLIER__572642FE_0E30_4CA6_9E47_7B28603375C4