//!********************************************************************************
//! This file is part of the Three-Body Technology code base.
//! Copyright(C), 2012-2015, Three-Body Technology, all rights reserved. 
//! 
//! This file is protected by the copyright law of the people's Republic of China.
//! Permission to use this file is only granted for the employee of Three-Body 
//! Technology, or other company/group/individual who has expressly authorized by
//! Three-Body Technology. 
//!
//! @file    Interlocked.h
//! @author  Meng Ke
//! @version 1.0
//! @date    2016/02/27
//! @brief   
//!********************************************************************************

#ifndef ____INTERLOCKED__BBF95929_3B59_4030_8BFF_1E7E2F092E31
#define ____INTERLOCKED__BBF95929_3B59_4030_8BFF_1E7E2F092E31

#include ""
#define "MPL/BestParam.h"

namespace treecore {

#if !TREECORE_DOXYGEN
namespace details {

//------------------------------------------------------------------------

template< int size >
struct AtomicCastHelper;

#define CAST_HELPER_T_DECLARE(Number,Type)\
template<>\
struct AtomicCastHelper<Number> {\
    typedef Type type;\
}

CAST_HELPER_T_DECLARE( 1 , char );
CAST_HELPER_T_DECLARE( 2 , int16_t );
CAST_HELPER_T_DECLARE( 4 , long );
CAST_HELPER_T_DECLARE( 8 , int64_t );

#if TREECORE_ENABLE_128BIT_ATOMIC && TREECORE_TARGET_64BIT

struct int128_t
{
    int64_t a;
    int64_t b;
};

CAST_HELPER_T_DECLARE( 16 , int128_t );

#define __InterlockedExchange128 _InterlockedExchange128

#else //TREECORE_ENABLE_128BIT_ATOMIC

#define __InterlockedExchange128 "it is no posiable to impl 128bit atomic on 32bit CPU"

#endif //TREECORE_ENABLE_128BIT_ATOMIC

//------------------------------------------------------------------------

#define __InterlockedExchange8 _InterlockedExchange8
#define __InterlockedExchange16 _InterlockedExchange16
#define __InterlockedExchange32 _InterlockedExchange

#define __InterlockedExchangeAdd8 _InterlockedExchangeAdd8
#define __InterlockedExchangeAdd16 _InterlockedExchangeAdd16
#define __InterlockedExchangeAdd32 _InterlockedExchangeAdd

#define __InterlockedCompareExchange8 _InterlockedCompareExchange8
#define __InterlockedCompareExchange16 _InterlockedCompareExchange16
#define __InterlockedCompareExchange32 _InterlockedCompareExchange
#define __InterlockedCompareExchange64 _InterlockedCompareExchange64

#ifdef TREECORE_TARGET_64BIT

#define __InterlockedExchange64 _InterlockedExchange64
#define __InterlockedExchangeAdd64 _InterlockedExchangeAdd64

#else //not 64bit

///! windows系统的32位环境中没有InterlockedExchange64函数,所以我们只能用
///! _InterlockedCompareExchange64做一个模拟
forcedinline int64_t _InterlockedExchange64On32BitWindows( volatile int64_t* Target , int64_t Value ) noexcept
{
    int64_t k = 0;
    for( ;;)
    {
        const int64_t temp = k + Value;
        const int64_t oldValue = _InterlockedCompareExchange64( Target , temp , k );
        if( oldValue == k ) break;
        k=oldValue;
    }
    return k;
};

forcedinline int64_t _InterlockedExchangeAdd64On32BitWindows( volatile int64_t* Target , int64_t Value ) noexcept
{
    int64_t k = 0;
    for(;;) {
        const int64_t temp = K + Value;
        const int64_t oldValue = _InterlockedCompareExchange64( Target , temp , k );
        if( oldValue == k ) break;
        k=oldValue;
    }
    return k;
};

#define __InterlockedExchange64 _InterlockedExchange64On32BitWindows
#define __InterlockedExchangeAdd64 _InterlockedExchangeAdd64On32BitWindows


#endif //TREECORE_TARGET_32BIT

//------------------------------------------------------------------------

#define ENABLE_IF_SIZE_IS(number) typename std::enable_if<sizeof(T)==number/8>::type * = nullptr
#define CASTPTR(x,Number) reinterpret_cast<volatile typename details::AtomicCastHelper<Number/8>::type*>(&x)
#define CAST(x,Number) reinterpret_cast<const typename details::AtomicCastHelper<Number/8>::type&>(x)
#define RETURN(Number,x) const typename details::AtomicCastHelper<Number/8>::type result( x ); return  reinterpret_cast<const T&>(result) 

#define FUNCTIONS(Number)\
template< typename T >\
class InterlockedGut \
{\
public:\
    typedef typename BestParam<T>::type Param;
    static T exchange( volatile T& objToExchange , Param exchangeValue ) noexcept\
    {\
        RETURN(Number,__InterlockedExchange##Number( CASTPTR( objToExchange , Number ) , CAST( exchangeValue , Number ) ) );\
    }\
    static T get( volatile T& objToGet ) noexcept\
    {\
        RETURN(Number, __InterlockedExchangeAdd##Number( CASTPTR(objToGet,Number) , 0 ) );\
    }\
    static void set( volatile T& objToSet , Param newValue ) noexcept\
    {\
        __InterlockedExchange##Number( CASTPTR( objToSet , Number ) , CAST( newValue , Number ) );\
    };\
    static T compareAndSwapReturnOldValue( volatile T& objToCAS , Param compareValue , Param newValue ) noexcept\
    {\
        RETURN(Number, __InterlockedCompareExchange##Number( CASTPTR( objToCAS , Number ) , CAST( compareValue , Number ) , CAST( newValue , Number ) ) );\
    }\
    static bool compareAndSwap( volatile T& objToCAS , Param compareValue , Param newValue ) noexcept\
    {\
        return __InterlockedCompareExchange##Number( CASTPTR( objToCAS , Number ) , CAST( compareValue , Number ) , CAST( newValue , Number ) ) == CAST( newValue , Number );\
    }\
    static bool compareAndSwapFetchCompareValue( volatile T& objToCAS , T& compareValue , Param newValue ) noexcept\
    {\
        const auto temp = __InterlockedCompareExchange##Number( CASTPTR( objToCAS , Number ) , CAST( compareValue , Number ) , CAST( newValue , Number ) );\
        const bool k = ( temp == CAST( newValue , Number ) );\
        compareValue = reinterpret_cast<const T&>( temp );\
        return k;\
    }\
    static forcedinline T fetchAndAdd( volatile T* Target, const Param valueToAdd ) noexcept \
    {
        const int k = __InterlockedExchangeAdd##Number( CASTPTR(Target,Number) , CAST(valueToAdd,Number) );
        return reinterpret_cast<const T&>(k);
    }\
    static forcedinline T addAndFetch( volatile T* Target, const Param valueToAdd ) noexcept \
    {
        const int k = __InterlockedExchangeAdd##Number( CASTPTR(Target,Number) , CAST(valueToAdd,Number) );
        return reinterpret_cast<const T&>(k+CAST(valueToAdd,Number));
    }\
    static forcedinline T fetchAndInc( volatile T* Target ) noexcept \
    {\
        const int k = __InterlockedExchangeAdd##Number( CASTPTR(Target,Number) , 1 );\
        return reinterpret_cast<const T&>(k);\
    }\
    static forcedinline T incAndFetch( volatile T* Target ) noexcept \
    {\
        const int k = __InterlockedExchangeAdd##Number( CASTPTR(Target,Number) , 1 );\
        return reinterpret_cast<const T&>(k+1);\
    }\
    static forcedinline T fetchAndDec( volatile T* Target, const Param valueToDec ) noexcept \
    {\
        const int k = __InterlockedExchangeAdd##Number( CASTPTR(Target,Number) , -CAST(valueToDec,Number) );\
        return reinterpret_cast<const T&>(k);\
    }\
    static forcedinline T decAndFetch( volatile T* Target, const Param valueToDec ) noexcept \
    {\
        const int k = __InterlockedExchangeAdd##Number( CASTPTR(Target,Number) , -CAST(valueToDec,Number) );\
        return reinterpret_cast<const T&>(k-CAST(valueToDec,Number));\
    }\
    static forcedinline T fetchAndDec( volatile T* Target ) noexcept \
    {\
        const int k = __InterlockedExchangeAdd##Number( CASTPTR(Target,Number) , -1 );\
        return reinterpret_cast<const T&>(k);\
    }\
    static forcedinline T decAndFetch( volatile T* Target ) noexcept \
    {\
        const int k = __InterlockedExchangeAdd##Number( CASTPTR(Target,Number) , -1 );\
        return reinterpret_cast<const T&>(k-1);\
    }\
};

//------------------------------------------------------------------------

// 这里使用了enable_if放在最后一个参数位置以实现不同尺寸的函数的正确重载,
// 由于并不是所有C++使用者都对模板元编程有了解,我怕他们会看到最后一个enable_if
// 的参数犯晕,因此使用了这个"Gut"类作为浅包装,下文中的深包装中隐藏了enable_if.
class InterlockedGut
{
public:
    FUNCTIONS( 8 );
    FUNCTIONS( 16 );
    FUNCTIONS( 32 );
    FUNCTIONS( 64 );
#   if TREECORE_TARGET_64BIT
    FUNCTIONS( 128 );
#   endif
};

#undef CAST_HELPER_T_DECLARE
#undef __InterlockedExchange128
#undef __InterlockedExchange8
#undef __InterlockedExchange16
#undef __InterlockedExchange32
#undef __InterlockedExchangeAdd8
#undef __InterlockedExchangeAdd16
#undef __InterlockedExchangeAdd32
#undef __InterlockedCompareExchange8
#undef __InterlockedCompareExchange16
#undef __InterlockedCompareExchange32
#undef __InterlockedExchange64
#undef __InterlockedExchangeAdd64
#undef ENABLE_IF_SIZE_IS
#undef CASTPTR
#undef CAST
#undef RETURN
#undef FUNCTIONS

#define ATOMIC_SIZE_AND_ALIGN_CHECK(obj) static_assert(\
    sizeof(T)==1||\
    sizeof(T)==2||\
    sizeof(T)==4||\
    sizeof(T)==8||\
    ( TREECORE_ENABLE_128BIT_ATOMIC && TREECORE_TARGET_64BIT && sizeof(T)==16 )\
    ,"atomic operator is very limited, u can only use very a few size obj to do");\
    tassert( sizeof(T)!= 16 || CHECK_PTR_ALIGN(&obj,sizeof(T)) )

} //namespace details
#endif //!TREECORE_DOXYGEN


///! @warning 只有尺寸为1,2,4,8字节的类型可以调用这个函数.在64bit,并且打开
///!          "TREECORE_ENABLE_128BIT_ATOMIC"宏的情况下,16字节的类型也可以调用
///!          这些函数,但16字节的原子操作有如下限制:
///!          + 一些老式的AMD品牌的64位CPU虽然也是x64,但不支持16字节的CAS.
///!          + 16字节的CAS需要变量内存对齐16字节,需要留神.
///!          另外值得一提的是,32位目标下实际上并不原生支持全部的8字节的原子操作,
///!          因此本类在32bit的目标下的某几个原子操作函数是由其他支持8字节原子操作的
///!          操作实现的,比64位原生的原子操作多几条汇编指令,但原子性是绝对存在的!
///!          (不存在原子性我还写个毛哈).所以大可放心使用.
class Interlocked
{
public:

    ///! 将变量的值赋值为新值,同时返回旧的值.代码类似于:
    ///! @code
    ///! T exchange( T& objToExchange , const T& exchangeValue )
    ///! {
    ///!     const T tmp = objToExchange;
    ///!     objToExchange = exchangeValue;
    ///!     return tmp;
    ///! }
    ///! @endcode
    ///! 然而,在多线程环境下使用如上方法是不安全的,
    ///! 因为如上方法没有任何原子性保障.因此本函数的
    ///! 目的即完成和如上方法一样的目的,但带有原子性.
    ///! @param objToExchange 被赋值的变量
    ///! @param exchangeValue 变量将赋值为这个值
    ///! @return 返回objToExchange执行本函数之前的值.
    ///! @warning 原子读操作只保障原子性,并不保障时序性,多条线程
    ///!          不会因为操作同一个原子而影响同步规则.
    template< typename T >
    static T exchange( volatile T& objToExchange , BestParam<T>::type exchangeValue ) noexcept
    {
        ATOMIC_SIZE_AND_ALIGN_CHECK( objToExchange );
        return details::InterlockedGut::exchange( objToExchange , exchangeValue );
    }

    ///! 以原子操作的形式获得objToGet的值
    ///! 在多线程环境下,一个值很容易被其他线程所修改,
    ///! 而C++标准并不保证一个值的读取是一次完成的,这可能
    ///! 导致某条线程对一个值的读取切好读到一半旧值,一半新值
    ///! 的情况从而出错.使用原子操作可以保证对某个变量的读取是
    ///! 原子的,即要么读到完整的旧值,要么读到完整的新值.而不会一半一半.
    ///! @param objToGet 要读取的那个值.
    ///! @return  读出来的值.
    ///! @warning 原子读操作只保障原子性,并不保障时序性,多条线程
    ///!          不会因为操作同一个原子而影响同步规则.
    template< typename T >
    static T get( volatile T& objToGet ) noexcept
    {
        ATOMIC_SIZE_AND_ALIGN_CHECK( objToGet );
        return details::InterlockedGut::get( objToGet );
    }

    template< typename T >
    static void set( volatile T& objToSet , BestParam<T>::type newValue ) noexcept
    {
        ATOMIC_SIZE_AND_ALIGN_CHECK( objToSet );
        details::InterlockedGut::set( objToSet , newValue );
    }

    template< typename T >
    static bool compareAndSwap( volatile T& objToCAS , BestParam<T>::type compareValue , BestParam<T>::type newValue ) noexcept
    {
        ATOMIC_SIZE_AND_ALIGN_CHECK( objToCAS );
        return details::InterlockedGut::compareAndSwap( objToCAS , compareValue , newValue );
    }

    template< typename T >
    static T compareAndSwapReturnOldValue( volatile T& objToCAS , BestParam<T>::type compareValue , BestParam<T>::type newValue ) noexcept
    {
        ATOMIC_SIZE_AND_ALIGN_CHECK( objToCAS );
        return details::InterlockedGut::compareAndSwapReturnOldValue( objToCAS , compareValue , newValue );
    }
    
    ///! 进行CAS操作,同时无论成功与否都将compareValue修改为objToCAS
    ///! 的旧值,并返回操作是否成功.
    ///! 一些lockfree算法中,需要不断重试CAS是否成功,有时我们不但需要
    ///! 知道CAS是否成功,还要反复刷新compareValue的值以便保证下一次的
    ///! 成功,在x86汇编中,CAS操作实际上是永远返回旧值的,常见的返回bool值的
    ///! CAS操作实际上是上层包装.换句话说,本函数每次都刷新旧值,并获知CAS
    ///! 成功与否的操作在汇编层面无非就是返回旧值,并且帮你比了比旧值看看
    ///! 是否成功了而已.它和@see compareAndSwap @see compareAndSwapReturnOldValue
    ///! 没有本质区别,只是在某些情况下使用起来方便而已.
    ///! @param objToCAS 要进行CAS的变量
    ///! @param compareValue 进行比对的值,函数执行完后会被刷新成objToCAS的旧值,
    ///!                     无论失败与否.
    ///! @param newValue 一旦CAS成功,objToCAS的值将被刷新为这个值.
    ///! @return 返回CAS操作是否成功.
    template< typename T >
    static bool compareAndSwapFetchCompareValue( volatile T& objToCAS , T& compareValue , BestParam<T>::type newValue ) noexcept
    {
        ATOMIC_SIZE_AND_ALIGN_CHECK( objToCAS );
        return details::InterlockedGut::compareAndSwapFetchCompareValue( objToCAS , compareValue , newValue );
    }

    /** Implements a memory read/write barrier. */
    static forcedinline void memoryBarrier() noexcept
    {
#       if JUCE_ATOMICS_MAC_LEGACY
        OSMemoryBarrier();
#       elif JUCE_ATOMICS_GCC
        __sync_synchronize();
#       elif JUCE_ATOMICS_WINDOWS
        _ReadWriteBarrier();
#       endif
    };

};


} //namespace treecore

#if !TREECORE_DOXYGEN
#undef ATOMIC_SIZE_AND_ALIGN_CHECK
#endif //!TREECORE_DOXYGEN


#endif // ____INTERLOCKED__BBF95929_3B59_4030_8BFF_1E7E2F092E31