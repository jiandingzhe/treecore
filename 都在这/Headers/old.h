
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




/****************************************************
宏后面写不写分号? 有些自定义宏想要使用者写有些则不想,
以下这个宏会强迫使用者写上一个分号,否则则会报错.
*****************************************************/
#if defined(TREECORE_COMPILER_MSVC) && !defined(TREECORE_COMPILER_DOXYGEN)
#   define TREECORE_MACRO_FORCED_SEMICOLON(...) \
   __pragma(warning(push)) \
   __pragma(warning(disable:4127)) \
   do { __VA_ARGS__ } while (false) \
   __pragma(warning(pop))
#else
#   define TREECORE_MACRO_FORCED_SEMICOLON(...) do { __VA_ARGS__ } while (false)
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




/*******************************************************
forcedinline and neverinline
********************************************************/
#if TREECORE_DEBUG || TREECORE_COMPILER_DOXYGEN
#   define forcedinline inline
#   define neverinline // "noinline" macro may defined by other lib, so define "neverinline" here.
#else
#   if TREECORE_COMPILER_MSVC
#       define forcedinline __forceinline
#       define neverinline __attribute__((noinline))
#   else
#       define forcedinline inline __attribute__((always_inline))
#       define neverinline __declspec(noinline)
#   endif
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
可使用这个宏在build的时候放出一条信息
***************************************************************/
#if defined(TREECORE_COMPILER_MSVC) && !defined(TREECORE_COMPILER_DOXYGEN)
#   define ____TREECORE_CUSTOM_COMPILER_MESSAGE(file, line, mess) message( file "(" TREECPRE_MAKE_STRING(line) ") : warning: " TREECPRE_MAKE_STRING(mess) )
#   define TREECORE_CUSTOM_COMPILER_MESSAGE(message) __pragma(____TREECORE_CUSTOM_COMPILER_MESSAGE(__FILE__, __LINE__, message));
#else
#   if !defined(TREECORE_COMPILER)
#       define ____TREECORE_CUSTOM_COMPILER_MESSAGE(mess) message(TREECPRE_MAKE_STRING(mess))
#   endif
#   define TREECORE_CUSTOM_COMPILER_MESSAGE(message) _Pragma(TREECPRE_MAKE_STRING(____TREECORE_CUSTOM_COMPILER_MESSAGE(message)));
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
#   define PACKED(...) __pragma(pack(push, 1)) __VA_ARGS__ __pragma(pack(pop))
#else
#   define PACKED(...) __VA_ARGS__ __attribute__((__packed__))
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

#if TREECORE_DEBUG
#   define TREECORE_LEAK_DETECTOR(OwnerClass) \
        friend class juce::LeakedObjectDetector<OwnerClass>; \
        juce::LeakedObjectDetector<OwnerClass> TREECORE_MACRO_JOIN(leakDetector, __LINE__)
#else
#   define TREECORE_LEAK_DETECTOR(OwnerClass)
#endif

#define JUCE_DECLARE_NON_COPYABLE(className) \
    className (const className&) =delete; \
    className& operator= (const className&) =delete

#define JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(className) \
    JUCE_DECLARE_NON_COPYABLE(className) \
    JUCE_LEAK_DETECTOR(className)


#include "tools/leakDetector.h"