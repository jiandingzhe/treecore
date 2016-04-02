
/**********************************************************************
查找编译器类型,TREECORE支持的编译器共有以下几种:
TREECORE_COMPILER_GCC : GCC
TREECORE_COMPILER_MSVC : 微软C++编译器
TREECORE_COMPILER_MINGW : minGW的编译器
TREECORE_COMPILER_ICC : Intel ICC编译器
TREECORE_COMPILER_CLANG : Clang编译器
TREECORE_COMPILER_DOXYGEN : DOXYGEN的解析器

请注意这些编译器宏并非是排它的,
其中,Clang编译器,minGW的编译器和ICC编译器要依托GCC或MSVC编译器,
因此有可能同时有多个编译器宏被指定,例如,在windows平台下使用
ICC编译器,则TREECORE_COMPILER_MSVC和TREECORE_COMPILER_ICC两个宏
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
#   define TREECORE_COMPILER_ICC 1
#endif

#ifdef __clang__
#   define TREECORE_COMPILER_CLANG 1
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
#if defined(TREECORE_COMPILER_MSVC) && !defined(TREECORE_COMPILER_ICC)
#   if _MSC_VER < 1900 //VS 2015
#   define TREECORE_COMPILER_NOT_GOOD
#endif

#if defined(TREECORE_COMPILER_GCC) && !defined(TREECORE_COMPILER_CLANG)
#   if ( __GNUC__ * 100 + __GNUC_MINOR__ ) < 308
#       define TREECORE_COMPILER_NOT_GOOD
#   endif
#endif

#if defined(TREECORE_COMPILER_CLANG)
#   if ( __clang_major__*100 + __clang_minor__ ) < 301
#       define TREECORE_COMPILER_NOT_GOOD
#   endif
#endif

#if defined(TREECORE_COMPILER_ICC)
#if defined(__INTEL_COMPILER)
#   if __INTEL_COMPILER < 1400
#       define TREECORE_COMPILER_NOT_GOOD
#   endif
#elif defined(__ICL)
#   if __ICL < 1400
#       define TREECORE_COMPILER_NOT_GOOD
#   endif
#elif defined(__ICC)
#   if __ICC < 1400
#       define TREECORE_COMPILER_NOT_GOOD
#   endif
#else
#   error can not know ICC version some how!
#endif

#ifdef TREECORE_COMPILER_NOT_GOOD
#   error TREECORE need a good C++11 compiler to build!
#   undef TREECORE_COMPILER_NOT_GOOD
#endif




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




/**************************************************
下面是对CPU的选择:
TREECORE_CPU_INTEL
TREECORE_CPU_ARM
**************************************************/
#if defined(__arm__) || defined(__arm) || defined(__arm64__) || defined (__arm64__) || defined( _M_ARM ) || defined(_M_ARMT)
#   define TREECORE_CPU_ARM 1
#elif defined (__ppc__) || defined (__ppc64__) || defined(__powerpc) || defined(_ARCH_PPC)
#   error ppc platform is beyond the design.
#else
#   define TREECORE_CPU_INTEL 1
#endif




/**************************************************
下面是对32位和64位的选择
**************************************************/
#if defined(_WIN64) || defined(__MINGW64__) || defined(__amd64__) || defined(__LLP64__) || defined(__LP64__) || defined(_LP64) || defined(__arm64__) || defined(__ARM_ARCH_ISA_A64)
#   define TREECORE_TARGET_64BIT 1
#else
#   define TREECORE_TARGET_32BIT 1
#endif

#if TREECORE_CPU_ARM && TREECORE_OS_WINDOWS
#   error there is no arm processor support plan on windows yet.
#elif TREECORE_CPU_ARM && TREECORE_OS_MACOSX
#   error something wrong, I don't know there is any version of Mac OSX running on ARM.
#elif TREECORE_CPU_INTEL && TREECORE_OS_IOS
#   error something wrong, I don't know there is any version of iOS running on x86.
#endif




/********************************************************

接下来我们要设置一些flag来区分一些SIMD指令集,在ARM上treecore需要
NEON指令集来进行一些手动优化,在intel上则最少需要SSE3指令集.同时,随着
用户开启不同的指令集,treecore会选择不同的指令集来进行某些代码的优化方案.
当然,选择的指令集越新,对用户的电脑CPU要求也越新.

* SSE3指令集最低在奔腾4的CPU上即支持了,一方面考虑到奔腾4已经是10年前的CPU了,
  另一方面SSE3提供了很多很有用的交叉平行化指令,因此我们不打算比这个再低了.

* iphone4的处理器ARMv7已经可以支持NEON指令集了,我们看不出还有任何
  在IOS上不使用NEON的理由.

* 某些Android可能使用了老的处理器而不支持NEON,在可预见的未来都没有支持
  他们的计划.

* __ARM_NEON__ is legacy, since it is not defined in the 
  ARM(R) C Language Extensions Release 1.1: 
  http://infocenter.arm.com/help/topic/com.arm.doc.ihi0053b/IHI0053B_arm_c_language_extensions_2013.pdf.
  Instead, __ARM_NEON should be used, see chapter 4.4 (ACLE headers).
********************************************************/
#if defined(TREECORE_CPU_ARM)
#   if defined(__ARM_NEON__) || defined(__ARM_NEON)
#       define TREECORE_INSTRUCTION_NEON 1
#   else
#       error can not build treecore on a ARM CPU without NEON.
#   endif
#else
#   if defined(__SSE__)
#       define TREECORE_INSTRUCTION_SSE1 1
#   endif
#   if defined(__SSE2__)
#       define TREECORE_INSTRUCTION_SSE2 1
#   endif
#   if defined(__SSE3__)
#       define TREECORE_INSTRUCTION_SSE3 1
#   endif
#   if defined(__SSSE3__)
#       define TREECORE_INSTRUCTION_SSSE3 1
#   endif
#   if defined(__SSE4_1__)
#       define TREECORE_INSTRUCTION_AVX41 1
#   endif
#   if defined(__SSE4_2__)
#       define TREECORE_INSTRUCTION_SSE42 1
#   endif
#   if defined(__AVX__)
#       define TREECORE_INSTRUCTION_AVX1 1
#   endif
#   if defined(__AVX2__)
#       define TREECORE_INSTRUCTION_AVX2 1
#   endif
#endif

#if !defined(TREECORE_INSTRUCTION_SSE1) || !defined(TREECORE_INSTRUCTION_SSE2) || !defined(TREECORE_INSTRUCTION_SSE3)
/*
    if you hit this, the most reason is you need to set SSE3 flag on your complier,
    like "-msse3" on GCC. most complier genrate bad asm code which is sooo slow if
    no SSE flag set. treecore need at least SSE3 instruction to build and now a day
    almost all CPU have SSE3. 
*/
#   error treecore need at least SSE3 instruction to build.
#endif




/***************************************************
下面我们检查是debug还是release
treecore在debug模式和release模式下很多行为会有所不同,
其中有很多会影响效率,因此务必确保debug和release的正确设置.
如果想在release模式下依然开启一些断言检查,可在项目设置中
定义"TREECORE_FORCE_DEBUG"宏.
***************************************************/
#if defined(__DEBUG) || defined(_DEBUG) || defined(DEBUG) || defined(__DEBUG__) || defined(TREECORE_FORCE_DEBUG)
#   define TREECORE_DEBUG 1
#endif
#if !defined(TREECORE_FORCE_DEBUG) && ( defined(__NDEBUG) || defined(_NDEBUG) || defined(NDEBUG) || defined(__NDEBUG__) )
#   define TREECORE_RELEASE 1
#endif
#if defined(TREECORE_DEBUG) && defined(TREECORE_RELEASE)
#   error both "release" and "debug" flags defined! can not know witch to use.
#endif
#if !defined(TREECORE_DEBUG) && !defined(TREECORE_RELEASE)
#   error both "release" and "debug" flags are NOT defined! can not know witch to use.
#endif




/****************************************************
 This macro defines the C calling convention used as 
 the standard for treecore calls. 
*****************************************************/
#if TREECORE_COMPILER_MSVC
 #define TREECORE_CALLTYPE __stdcall
 #define TREECORE_CDECL __cdecl
#else
 #define TREECORE_CALLTYPE
 #define TREECORE_CDECL
#endif




/*****************************************************
A good old-fashioned C macro concatenation helper.
This combines two items (which may themselves be macros) into a single string,
avoiding the pitfalls of the ## macro operator.
******************************************************/
#if !defined(TREECORE_COMPILER_DOXYGEN)
 #define ____TREECORE_JOIN_MACRO(a, b) a##b
 #define ____TREECPRE_MAKE_STRING(a) #a
#endif
#define TREECORE_MACRO_JOIN(a,b) ____TREECORE_JOIN_MACRO(a,b)
#define TREECPRE_MAKE_STRING(x) ____TREECPRE_MAKE_STRING(x)




/*******************************************************
Cross-compiler deprecation macros..
********************************************************/
#if defined(TREECORE_COMPILER_MSVC)
#   define TREECORE_DEPRECATED __declspec(deprecated)
#elif defined(TREECORE_COMPILER_GCC)
#   define TREECORE_DEPRECATED __attribute__((__deprecated__)) //they all go in the same place in the syntax! 2012
#else
#   error something wrong
#endif




/************************************************************
一个告知静态分析器某个函数永远不会return的宏
************************************************************/
#if TREECORE_COMPILER_CLANG && defined (__has_feature)
#   if __has_feature(attribute_analyzer_noreturn)
        inline void __attribute__((analyzer_noreturn)) ____treecore_noreturn_helper_fun() {}
#       define TREECORE_TELL_STATIC_ANALYZER_NORETURN ____treecore_noreturn_helper_fun();
#   endif
#else
#   define TREECORE_TELL_STATIC_ANALYZER_NORETURN
#endif


/*************************************************************** 
This will try to break into the debugger if the app is currently being debugged.
If called by an app that's not being debugged, the behaiour isn't defined - it may crash or not, depending
on the platform.
***************************************************************/
#if defined(TREECORE_CPU_ARM)
#    define treecore_debugBreak { ::kill (0, SIGTRAP); }
#elif defined(TREECORE_COMPILER_MSVC)
#   ifndef TREECORE_COMPILER_ICC
#       pragma intrinsic (__debugbreak)
#   endif
#   define treecore_debugBreak { __debugbreak(); }
#else
#   define treecore_debugBreak{ asm ("int $3"); }
#endif

#if JUCE_LOG_ASSERTIONS || JUCE_DEBUG
#   define TREECORE_LogCurrentAssertion treecore::logAssertion (__FILE__, __LINE__);
#else
#   define TREECORE_LogCurrentAssertion
#endif

#define tassertfalse TREECORE_MACRO_FORCED_SEMICOLON( { TREECORE_LogCurrentAssertion; treecore_debugBreak; TREECORE_TELL_STATIC_ANALYZER_NORETURN } ) 

#if TREECORE_DEBUG
#   define tassert(expression) TREECORE_MACRO_FORCED_SEMICOLON( if(!(expression)) tassertfalse; )
#   define run_with_check(experssion) tassert(experssion)
#   define TREECORE_DBGCODE(...) __VA_ARGS__
#else
#   define tassert(expression)
#   define run_with_check(experssion) experssion
#   define TREECORE_DBGCODE(...)
#endif









/***************************************************************
结构体的参数对齐,用法:
@code
PACKED(
struct foo
{
	int bar;
});
@endcode
***************************************************************/
#if defined(TREECORE_COMPILER_MSVC)
#   define TREECORE_PACKED(...) __pragma(pack(push, 1)) __VA_ARGS__ __pragma(pack(pop))
#else
#   define TREECORE_PACKED(...) __VA_ARGS__ __attribute__((__packed__))
#endif




/****************************************************************
This macro can be added to class definitions to disable the use of new/delete to
allocate the object on the heap, forcing it to only be used as a stack or member variable.
****************************************************************/
#define TREECORE_NO_HEAP_ALLOCATION \
    static void* operator new(size_t) =delete; \
    static void operator delete(void*) =delete; \
    static void* operator new[](size_t) =delete; \
    static void operator delete[](void*) =delete




/****************************************************************
This is a shorthand macro for declaring stubs for a class's copy constructor and operator=.

For example, instead of
@code
class MyClass
{
    etc..
private:
    MyClass (const MyClass&);
    MyClass& operator= (const MyClass&);
};@endcode

..you can just write:

@code
class MyClass
{
    etc..
private:
    JUCE_DECLARE_NON_COPYABLE (MyClass)
};@endcode
*/
#define TREECORE_NO_COPYABLE(className) \
    className (const className&) =delete; \
    className& operator= (const className&) =delete




/********************************************************
 *  NEVER_TOUCH宏
 ********************************************************/
#if TREECORE_DEBUG
#   if TREECORE_COMPILER_MSVC
#      define TREECORE_UNREACHABLE { tassertfalse; __assume(0); }
#   else
#      define TREECORE_UNREACHABLE { tassertfalse; __builtin_unreachable(); }
#   endif
#else
#   if TREECORE_COMPILER_MSVC
#       define TREECORE_UNREACHABLE { __assume(0); }
#   else
#       define TREECORE_UNREACHABLE { __builtin_unreachable(); }
#   endif
#endif


#define TREECORE_IS_TRIVIAL(classname) std::is_trivial<classname>::value
#define TREECORE_IS_POD(classname) std::is_pod<classname>::value
#define TREECORE_IS_STANDARDLAYOUT(classname) std::is_standard_layout<classname>::value
#define TREECORE_IS_DEFAULTCOPYABLE(classname) IS_TRIVIAL(classname) //todo: mingw这里有问题,得研究一下标准库

#define TREECORE_CHECK_POD(classname) static_assert(TREECORE_IS_POD(classname),"")
#define TREECORE_CHECK_TRIVIAL(classname) static_assert(TREECORE_IS_TRIVIAL(classname),"")
#define TREECORE_CHECK_DEFAULTCOPYABLE(classname) static_assert(TREECORE_IS_DEFAULTCOPYABLE(classname),"")
#define TREECORE_CHECK_POD(classname) static_assert(TREECORE_IS_POD(classname),"")
#define TREECORE_CHECK_POD(classname) static_assert(TREECORE_IS_POD(classname),"")




/**
  the check is implemented in malloc.c in the GNU C Library (glibc), 
  since malloc.c only care about positive numbers, they write 
  @code
  ((x != 0) && !(x & (x - 1)))
  @endcode
  there. but we care neg numbers, so make a simple change by 
  changing x!=0 to x>0. neg numbers can never be power of 2 anyway.
*/
#define TREECORE_IS_POWER_OF_TWO(x) ((x > 0) && !(x & (x - 1)))
#define makePowerTwo(x) (1u<<(x))
#define modPowerTwo(x,number) ((x)&(number-1))
#define divPowerTwo(x,number) ((x)>>(number-1))
#define floorPowTwo(x,number) ((x)&(~(number-1)))
#define ceilPowTwo(x,number) floorPowTwo( x+1,number )

#define makeSIMD(x,k) ( (x)&(~((decltype(x))(k-1))) )




#define TREECORE_CHECK_RANGE(x,minLimit,maxLimit) tassert( (x)>=minLimit && (x)<maxLimit )



//* 非规格化小数
#define isDenormalFloat(x) (std::fabs(x)==0.f||std::fabs(x)>=FLOAT_NORMAL_MIN)
#define isDenormalDouble(x) (std::abs(x)==0.0||std::abs(x)>=DOUBLE_NORMAL_MIN)
#define checkDenormalFloat(x) tassert(isDenormalFloat(x))
#define checkDenormalDouble(x) tassert(isDenormalDouble(x))




#define BIGGER_TYPE TypeIf< sizeof(T1)<=sizeof(T2) , T2, T1 >::type
template< typename T1 , typename T2 >
forcedinline typename BIGGER_TYPE tmax( const T1 a, const T2 b ) noexcept
{
    return ( (BIGGER_TYPE)a < (BIGGER_TYPE)b) ? (BIGGER_TYPE)b : (BIGGER_TYPE)a; 
}
#undef BIGGER_TYPE

template< typename T1 , typename... OtherTs >
forcedinline T tmax( const T a , const OtherTs... others ) noexcept
{
    return tmax(a,tmax(others));
}
//这里还缺有符号无符号


#define double_Pi 3.1415926535897932384626433832795
#define float_Pi  3.14159265358979323846f
#define double_lnN ?
#define float_lnN ?

/*
  ==============================================================================

   This file is part of the juce_core module of the JUCE library.
   Copyright (c) 2013 - Raw Material Software Ltd.

   Permission to use, copy, modify, and/or distribute this software for any purpose with
   or without fee is hereby granted, provided that the above copyright notice and this
   permission notice appear in all copies.

   THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH REGARD
   TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS. IN
   NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL
   DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER
   IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN
   CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.

   ------------------------------------------------------------------------------

   NOTE! This permissive ISC license applies ONLY to files within the juce_core module!
   All other JUCE modules are covered by a dual GPL/commercial license, so if you are
   using any other modules, be sure to check that you also comply with their license.

   For more details, visit www.juce.com

  ==============================================================================
*/

#ifndef JUCE_MATHSFUNCTIONS_H_INCLUDED
#define JUCE_MATHSFUNCTIONS_H_INCLUDED

namespace juce
{

//==============================================================================
/*
    This file sets up some handy mathematical typdefs and functions.
*/

//==============================================================================
// Definitions for the int8, int16, int32, int64 and pointer_sized_int types.



#ifndef DOXYGEN
 /** A macro for creating 64-bit literals.
     Historically, this was needed to support portability with MSVC6, and is kept here
     so that old code will still compile, but nowadays every compiler will support the
     LL and ULL suffixes, so you should use those in preference to this macro.
 */
 #define literal64bit(longLiteral)     (longLiteral##LL)
#endif

#if JUCE_64BIT
  /** A signed integer type that's guaranteed to be large enough to hold a pointer without truncating it. */
  typedef int64                     pointer_sized_int;
  /** An unsigned integer type that's guaranteed to be large enough to hold a pointer without truncating it. */
  typedef uint64                    pointer_sized_uint;
#elif JUCE_MSVC
  /** A signed integer type that's guaranteed to be large enough to hold a pointer without truncating it. */
  typedef _W64 int                  pointer_sized_int;
  /** An unsigned integer type that's guaranteed to be large enough to hold a pointer without truncating it. */
  typedef _W64 unsigned int         pointer_sized_uint;
#else
  /** A signed integer type that's guaranteed to be large enough to hold a pointer without truncating it. */
  typedef int                       pointer_sized_int;
  /** An unsigned integer type that's guaranteed to be large enough to hold a pointer without truncating it. */
  typedef unsigned int              pointer_sized_uint;
#endif

//#if JUCE_MSVC
  typedef pointer_sized_int ssize_t;
//#endif

//==============================================================================
// Some indispensible min/max functions

/** Returns the larger of two values. */
template <typename Type>
inline Type jmax (const Type a, const Type b)                                               { return (a < b) ? b : a; }

/** Returns the larger of three values. */
template <typename Type>
inline Type jmax (const Type a, const Type b, const Type c)                                 { return (a < b) ? ((b < c) ? c : b) : ((a < c) ? c : a); }

/** Returns the larger of four values. */
template <typename Type>
inline Type jmax (const Type a, const Type b, const Type c, const Type d)                   { return jmax (a, jmax (b, c, d)); }

/** Returns the smaller of two values. */
template <typename Type>
inline Type jmin (const Type a, const Type b)                                               { return (b < a) ? b : a; }

/** Returns the smaller of three values. */
template <typename Type>
inline Type jmin (const Type a, const Type b, const Type c)                                 { return (b < a) ? ((c < b) ? c : b) : ((c < a) ? c : a); }

/** Returns the smaller of four values. */
template <typename Type>
inline Type jmin (const Type a, const Type b, const Type c, const Type d)                   { return jmin (a, jmin (b, c, d)); }

/** Remaps a normalised value (between 0 and 1) to a target range.
    This effectively returns (targetRangeMin + value0To1 * (targetRangeMax - targetRangeMin))
*/
template <class Type>
static Type jmap (Type value0To1, Type targetRangeMin, Type targetRangeMax)
{
    return targetRangeMin + value0To1 * (targetRangeMax - targetRangeMin);
}

/** Remaps a value from a source range to a target range. */
template <class Type>
static Type jmap (Type sourceValue, Type sourceRangeMin, Type sourceRangeMax, Type targetRangeMin, Type targetRangeMax)
{
    return targetRangeMin + ((targetRangeMax - targetRangeMin) * (sourceValue - sourceRangeMin)) / (sourceRangeMax - sourceRangeMin);
}

/** Scans an array of values, returning the minimum value that it contains. */
template <typename Type>
const Type findMinimum (const Type* data, int numValues)
{
    if (numValues <= 0)
        return Type();

    Type result (*data++);

    while (--numValues > 0) // (> 0 rather than >= 0 because we've already taken the first sample)
    {
        const Type& v = *data++;
        if (v < result)  result = v;
    }

    return result;
}

/** Scans an array of values, returning the maximum value that it contains. */
template <typename Type>
const Type findMaximum (const Type* values, int numValues)
{
    if (numValues <= 0)
        return Type();

    Type result (*values++);

    while (--numValues > 0) // (> 0 rather than >= 0 because we've already taken the first sample)
    {
        const Type& v = *values++;
        if (result < v)  result = v;
    }

    return result;
}

/** Scans an array of values, returning the minimum and maximum values that it contains. */
template <typename Type>
void findMinAndMax (const Type* values, int numValues, Type& lowest, Type& highest)
{
    if (numValues <= 0)
    {
        lowest = Type();
        highest = Type();
    }
    else
    {
        Type mn (*values++);
        Type mx (mn);

        while (--numValues > 0) // (> 0 rather than >= 0 because we've already taken the first sample)
        {
            const Type& v = *values++;

            if (mx < v)  mx = v;
            if (v < mn)  mn = v;
        }

        lowest = mn;
        highest = mx;
    }
}


//==============================================================================
/** Constrains a value to keep it within a given range.

    This will check that the specified value lies between the lower and upper bounds
    specified, and if not, will return the nearest value that would be in-range. Effectively,
    it's like calling jmax (lowerLimit, jmin (upperLimit, value)).

    Note that it expects that lowerLimit <= upperLimit. If this isn't true,
    the results will be unpredictable.

    @param lowerLimit           the minimum value to return
    @param upperLimit           the maximum value to return
    @param valueToConstrain     the value to try to return
    @returns    the closest value to valueToConstrain which lies between lowerLimit
                and upperLimit (inclusive)
    @see jlimit0To, jmin, jmax
*/
template <typename Type>
inline Type jlimit (const Type lowerLimit,
                    const Type upperLimit,
                    const Type valueToConstrain) noexcept
{
    jassert (lowerLimit <= upperLimit); // if these are in the wrong order, results are unpredictable..

    return (valueToConstrain < lowerLimit) ? lowerLimit
                                           : ((upperLimit < valueToConstrain) ? upperLimit
                                                                              : valueToConstrain);
}

/** Returns true if a value is at least zero, and also below a specified upper limit.
    This is basically a quicker way to write:
    @code valueToTest >= 0 && valueToTest < upperLimit
    @endcode
*/
template <typename Type>
inline bool isPositiveAndBelow (Type valueToTest, Type upperLimit) noexcept
{
    jassert (Type() <= upperLimit); // makes no sense to call this if the upper limit is itself below zero..
    return Type() <= valueToTest && valueToTest < upperLimit;
}

template <>
inline bool isPositiveAndBelow (const int valueToTest, const int upperLimit) noexcept
{
    jassert (upperLimit >= 0); // makes no sense to call this if the upper limit is itself below zero..
    return static_cast <unsigned int> (valueToTest) < static_cast <unsigned int> (upperLimit);
}

/** Returns true if a value is at least zero, and also less than or equal to a specified upper limit.
    This is basically a quicker way to write:
    @code valueToTest >= 0 && valueToTest <= upperLimit
    @endcode
*/
template <typename Type>
inline bool isPositiveAndNotGreaterThan (Type valueToTest, Type upperLimit) noexcept
{
    jassert (Type() <= upperLimit); // makes no sense to call this if the upper limit is itself below zero..
    return Type() <= valueToTest && valueToTest <= upperLimit;
}

template <>
inline bool isPositiveAndNotGreaterThan (const int valueToTest, const int upperLimit) noexcept
{
    jassert (upperLimit >= 0); // makes no sense to call this if the upper limit is itself below zero..
    return static_cast <unsigned int> (valueToTest) <= static_cast <unsigned int> (upperLimit);
}

//==============================================================================
/** Handy function to swap two values. */
template <typename Type>
inline void swapVariables (Type& variable1, Type& variable2)
{
    std::swap (variable1, variable2);
}

/** Handy function for avoiding unused variables warning. */
template <typename Type1>
void ignoreUnused (const Type1&) noexcept {}

template <typename Type1, typename Type2>
void ignoreUnused (const Type1&, const Type2&) noexcept {}

template <typename Type1, typename Type2, typename Type3>
void ignoreUnused (const Type1&, const Type2&, const Type3&) noexcept {}

template <typename Type1, typename Type2, typename Type3, typename Type4>
void ignoreUnused (const Type1&, const Type2&, const Type3&, const Type4&) noexcept {}

/** Handy function for getting the number of elements in a simple const C array.
    E.g.
    @code
    static int myArray[] = { 1, 2, 3 };

    int numElements = numElementsInArray (myArray) // returns 3
    @endcode
*/
template <typename Type, int N>
inline int numElementsInArray (Type (&array)[N])
{
    (void) array; // (required to avoid a spurious warning in MS compilers)
    (void) sizeof (0[array]); // This line should cause an error if you pass an object with a user-defined subscript operator
    return N;
}

//==============================================================================
// Some useful maths functions that aren't always present with all compilers and build settings.

/** Using juce_hypot is easier than dealing with the different types of hypot function
    that are provided by the various platforms and compilers. */
template <typename Type>
inline Type juce_hypot (Type a, Type b) noexcept
{
   #if JUCE_MSVC
    return static_cast<Type> (_hypot (a, b));
   #else
    return static_cast<Type> (hypot (a, b));
   #endif
}

#ifndef DOXYGEN
template <>
inline float juce_hypot (float a, float b) noexcept
{
   #if JUCE_MSVC
    return (_hypotf (a, b));
   #else
    return (hypotf (a, b));
   #endif
}
#endif

/** 64-bit abs function. */
inline int64 abs64 (const int64 n) noexcept
{
    return (n >= 0) ? n : -n;
}

#if JUCE_MSVC && ! defined (DOXYGEN)  // The MSVC libraries omit these functions for some reason...
 template<typename Type> Type asinh (Type x) noexcept  { return std::log (x + std::sqrt (x * x + (Type) 1)); }
 template<typename Type> Type acosh (Type x) noexcept  { return std::log (x + std::sqrt (x * x - (Type) 1)); }
 template<typename Type> Type atanh (Type x) noexcept  { return (std::log (x + (Type) 1) - std::log (((Type) 1) - x)) / (Type) 2; }
#endif




//==============================================================================
/** The isfinite() method seems to vary between platforms, so this is a
    platform-independent function for it.
*/
template <typename NumericType>
inline bool juce_isfinite (NumericType) noexcept
{
    return true; // Integer types are always finite
}

template <>
inline bool juce_isfinite (float value) noexcept
{
   #if JUCE_MSVC
    return _finite (value) != 0;
   #else
    return std::isfinite (value);
   #endif
}

template <>
inline bool juce_isfinite (double value) noexcept
{
   #if JUCE_MSVC
    return _finite (value) != 0;
   #else
    return std::isfinite (value);
   #endif
}

//==============================================================================
#if JUCE_MSVC
 #pragma optimize ("t", off)
 #ifndef __INTEL_COMPILER
  #pragma float_control (precise, on, push)
 #endif
#endif

/** Fast floating-point-to-integer conversion.

    This is faster than using the normal c++ cast to convert a float to an int, and
    it will round the value to the nearest integer, rather than rounding it down
    like the normal cast does.

    Note that this routine gets its speed at the expense of some accuracy, and when
    rounding values whose floating point component is exactly 0.5, odd numbers and
    even numbers will be rounded up or down differently.
*/
template <typename FloatType>
inline int roundToInt (const FloatType value) noexcept
{
  #ifdef __INTEL_COMPILER
   #pragma float_control (precise, on, push)
  #endif

    union { int asInt[2]; double asDouble; } n;
    n.asDouble = ((double) value) + 6755399441055744.0;

   #if JUCE_BIG_ENDIAN
    return n.asInt [1];
   #else
    return n.asInt [0];
   #endif
}

inline int roundToInt (int value) noexcept
{
    return value;
}

#if JUCE_MSVC
 #ifndef __INTEL_COMPILER
  #pragma float_control (pop)
 #endif
 #pragma optimize ("", on)  // resets optimisations to the project defaults
#endif

/** Fast floating-point-to-integer conversion.

    This is a slightly slower and slightly more accurate version of roundDoubleToInt(). It works
    fine for values above zero, but negative numbers are rounded the wrong way.
*/
inline int roundToIntAccurate (const double value) noexcept
{
   #ifdef __INTEL_COMPILER
    #pragma float_control (pop)
   #endif

    return roundToInt (value + 1.5e-8);
}

/** Fast floating-point-to-integer conversion.

    This is faster than using the normal c++ cast to convert a double to an int, and
    it will round the value to the nearest integer, rather than rounding it down
    like the normal cast does.

    Note that this routine gets its speed at the expense of some accuracy, and when
    rounding values whose floating point component is exactly 0.5, odd numbers and
    even numbers will be rounded up or down differently. For a more accurate conversion,
    see roundDoubleToIntAccurate().
*/
inline int roundDoubleToInt (const double value) noexcept
{
    return roundToInt (value);
}

/** Fast floating-point-to-integer conversion.

    This is faster than using the normal c++ cast to convert a float to an int, and
    it will round the value to the nearest integer, rather than rounding it down
    like the normal cast does.

    Note that this routine gets its speed at the expense of some accuracy, and when
    rounding values whose floating point component is exactly 0.5, odd numbers and
    even numbers will be rounded up or down differently.
*/
inline int roundFloatToInt (const float value) noexcept
{
    return roundToInt (value);
}

//==============================================================================
/** Returns true if the specified integer is a power-of-two.
*/
template <typename IntegerType>
bool isPowerOfTwo (IntegerType value)
{
   return (value & (value - 1)) == 0;
}

/** Returns the smallest power-of-two which is equal to or greater than the given integer.
*/
inline int nextPowerOfTwo (int n) noexcept
{
    --n;
    n |= (n >> 1);
    n |= (n >> 2);
    n |= (n >> 4);
    n |= (n >> 8);
    n |= (n >> 16);
    return n + 1;
}

/** Returns the number of bits in a 32-bit integer. */
inline int countNumberOfBits (uint32 n) noexcept
{
    n -= ((n >> 1) & 0x55555555);
    n =  (((n >> 2) & 0x33333333) + (n & 0x33333333));
    n =  (((n >> 4) + n) & 0x0f0f0f0f);
    n += (n >> 8);
    n += (n >> 16);
    return (int) (n & 0x3f);
}

/** Returns the number of bits in a 64-bit integer. */
inline int countNumberOfBits (uint64 n) noexcept
{
    return countNumberOfBits ((uint32) n) + countNumberOfBits ((uint32) (n >> 32));
}

/** Performs a modulo operation, but can cope with the dividend being negative.
    The divisor must be greater than zero.
*/
template <typename IntegerType>
IntegerType negativeAwareModulo (IntegerType dividend, const IntegerType divisor) noexcept
{
    jassert (divisor > 0);
    dividend %= divisor;
    return (dividend < 0) ? (dividend + divisor) : dividend;
}

/** Returns the square of its argument. */
template <typename NumericType>
NumericType square (NumericType n) noexcept
{
    return n * n;
}

//==============================================================================
#if JUCE_INTEL || defined (DOXYGEN)
 /** This macro can be applied to a float variable to check whether it contains a denormalised
     value, and to normalise it if necessary.
     On CPUs that aren't vulnerable to denormalisation problems, this will have no effect.
 */
 #define JUCE_UNDENORMALISE(x)   { (x) += 0.1f; (x) -= 0.1f; }
#else
 #define JUCE_UNDENORMALISE(x)
#endif

//==============================================================================
/** This namespace contains a few template classes for helping work out class type variations.
*/
namespace TypeHelpers
{
   #if JUCE_VC8_OR_EARLIER
    #define PARAMETER_TYPE(type) const type&
   #else
    /** The ParameterType struct is used to find the best type to use when passing some kind
        of object as a parameter.

        Of course, this is only likely to be useful in certain esoteric template situations.

        Because "typename TypeHelpers::ParameterType<SomeClass>::type" is a bit of a mouthful, there's
        a PARAMETER_TYPE(SomeClass) macro that you can use to get the same effect.

        E.g. "myFunction (PARAMETER_TYPE (int), PARAMETER_TYPE (MyObject))"
        would evaluate to "myfunction (int, const MyObject&)", keeping any primitive types as
        pass-by-value, but passing objects as a const reference, to avoid copying.
    */
    template <typename Type> struct ParameterType                   { typedef const Type& type; };

   #if ! DOXYGEN
    template <typename Type> struct ParameterType <Type&>           { typedef Type& type; };
    template <typename Type> struct ParameterType <Type*>           { typedef Type* type; };
    template <>              struct ParameterType <char>            { typedef char type; };
    template <>              struct ParameterType <unsigned char>   { typedef unsigned char type; };
    template <>              struct ParameterType <short>           { typedef short type; };
    template <>              struct ParameterType <unsigned short>  { typedef unsigned short type; };
    template <>              struct ParameterType <int>             { typedef int type; };
    template <>              struct ParameterType <unsigned int>    { typedef unsigned int type; };
    template <>              struct ParameterType <long>            { typedef long type; };
    template <>              struct ParameterType <unsigned long>   { typedef unsigned long type; };
    template <>              struct ParameterType <int64>           { typedef int64 type; };
    template <>              struct ParameterType <uint64>          { typedef uint64 type; };
    template <>              struct ParameterType <bool>            { typedef bool type; };
    template <>              struct ParameterType <float>           { typedef float type; };
    template <>              struct ParameterType <double>          { typedef double type; };
   #endif

    /** A helpful macro to simplify the use of the ParameterType template.
        @see ParameterType
    */
    #define PARAMETER_TYPE(a)    typename TypeHelpers::ParameterType<a>::type
   #endif


    /** These templates are designed to take a type, and if it's a double, they return a double
        type; for anything else, they return a float type.
    */
    template <typename Type> struct SmallestFloatType             { typedef float  type; };
    template <>              struct SmallestFloatType <double>    { typedef double type; };
}


//==============================================================================

}

#endif   // JUCE_MATHSFUNCTIONS_H_INCLUDED


#if JUCE_MSVC
# define ZTD_NOINLINE __declspec(noinline)
# define SELECT_ANY __declspec( selectany )
# define RESTRICT __restrict
# define ASSUME(cond) __assume(cond)
# define ASSUME_PTR_SIMD(x) __assume(((intptr_t)x)%16==0)
# define ASSUME_SIZE_SIMD(x) __assume(x%4==0)
# define ALIGNOF(x) alignment_of<x>::value //__alignof(x)

# pragma warning(disable:4127) //条件表达式是常量
# pragma warning(disable:4822) //没有函数体

#else
# define ZTD_NOINLINE __attribute__ ((noinline))
# define SELECT_ANY __attribute__((selectany))
# define RESTRICT __restrict__
# define ASSUME(cond) do { if (!(cond)) __builtin_unreachable(); } while (0)
# define ASSUME_PTR_SIMD(x) __builtin_assume_aligned(x,16)
# define ASSUME_SIZE_SIMD(x) ASSUME(x%4==0)
# define ALIGNOF(x) __alignof__(x)
#endif

#if defined(__clang__) && ( __clang_major__ < 3 || (__clang_major__ == 3 && __clang_minor__ <= 5) )
#   undef ASSUME_PTR_SIMD
#   define ASSUME_PTR_SIMD(x)
#endif

#if JUCE_CLANG || JUCE_GCC
# undef SELECT_ANY
# define SELECT_ANY
#endif


class var
{
public:

    var get( ID name );

    template< typename T >
    void set( ID name , const BestParam<T>::type value )
    {

    }

    bool has( ID name ) const;

    template< typename... Args >
    var call( const Args&&... args )
    {

    }
CONFIG  += console release //少一个宏定义
};

class TObject
{
public:
    TObject();

};

int main()
{
    TreefaceApp app;
    Window mainWindow;
    mainWindow.show();
    return app.runLoop();
}

int main()
{
    TREEFACE_INIT;
    MessageManager::runLoop();
    //SingletonManager singletonManager; 这也是单例啊.....
    //MessageManager messageManager; 这个也是单例啊...
    Window mainWindow;
    mainWindow.show();
    return 
}

???????????????????????????????



