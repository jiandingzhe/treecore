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