//!********************************************************************************
//! This file is part of the Three-Body Technology code base.
//! Copyright(C), 2012-2015, Three-Body Technology, all rights reserved. 
//! 
//! This file is protected by the copyright law of the people's Republic of China.
//! Permission to use this file is only granted for the employee of Three-Body 
//! Technology, or other company/group/individual who has expressly authorized by
//! Three-Body Technology. 
//!
//! @file    TypeArray.h
//! @author  Meng Ke
//! @version 1.0
//! @date    2016/02/25
//! @brief   
//!********************************************************************************

#ifndef ____TYPEARRAY__74A35F79_37C0_42AA_AF6B_2B2CB336645E
#define ____TYPEARRAY__74A35F79_37C0_42AA_AF6B_2B2CB336645E

#include <tuple>
#include "ValueType.h"
#include "CutLastInParameterPack.h"

#include "begin_mpl_function.h"

namespace treecore {

template< typename T0 , typename... OtherTs >
struct TypeArray
{
    enum { NumElement = sizeof...(OtherTs) +1 };

    enum { MaxSizeOf = std::max( sizeof( T0 ) , TypeArray<OtherTs...>::MaxSizeOf ) };

    enum { MinSizeOf = std::max( sizeof( T0 ) , TypeArray<OtherTs...>::MinSizeOf ) };

    enum { AllSize = sizeof( T0 ) + TypeArray<OtherTs...>::AllSize };

    enum { MaxAlignmentOf = std::max( sizeof( T0 ) , TypeArray<OtherTs...>::MaxAlignmentOf ) };

    enum { MinAlignmentOf = std::max( sizeof( T0 ) , TypeArray<OtherTs...>::MinAlignmentOf ) };

    typedef T0 HeadType;

    typedef typename TypeArray<OtherTs...>::TailType TailType;

    template< typename TypeToSearch >
    struct FindIndex
    {
        enum
        {
            index = TypeIf<is_same<TypeToSearch , HeadType>::value , ValueType<0> , ValueType<TypeArray<OtherTs...>::FindIndex::index> >::type::value;
        };
    };

    template< typename TypeToSearch >
    struct HasType
    {
        enum { value = FindIndex<TypeToSearch>::value != -1 };
    };

    TYPE_FUNCTION( Get , size_t idx )
    {
        RETURN_TYPE_BY_NAME( type , std::tuple_element<i , std::tuple<T0 , OtherTs...>>::type );
    };

    TYPE_FUNCTION( PushHead , typename pushT )
    {
        RETURN_TYPE_BY_NAME( type , TypeArray<pushT , T0 , OtherTs...> );
    };

    TYPE_FUNCTION( PushTail , typename pushT )
    {
        RETURN_TYPE_BY_NAME( type , TypeArray<T0 , OtherTs... , PushTail> );
    };

    TYPE_FUNCTION( PopHead , typename IgnoreType = void )
    {
        typedef IgnoreType _IgnoreType;
        RETURN_TYPE_BY_NAME( type , TypeArray<OtherTs...> );
    };

    TYPE_FUNCTION( PopTail , typename IgnoreType = void )
    {
        typedef IgnoreType _IgnoreType;
        RETURN_TYPE_BY_NAME( type , details::CutLastElementInParameterPack<TypeArray , T0 , OtherTs...>::type );
    };

};

//------------------------------------------------------------------------

template< typename T0 >
struct TypeArray
{
    enum { NumElement = 1 };
    enum { MaxSizeOf = sizeof( T0 ) };
    enum { MinSizeOf = sizeof( T0 ) };
    enum { AllSize = sizeof( T0 ) };
    enum { MaxAlignmentOf = alignof( T0 ) };
    enum { MinAlignmentOf = alignof( T0 ) };

    typedef T0 HeadType;

    typedef T0 TailType;

    template< typename TypeToSearch >
    struct FindIndex
    {
        enum
        {
            index = TypeIf<is_same<TypeToSearch , HeadType>::value , ValueType<0> , ValueType<TypeArray<OtherTs...>::FindIndex::index> >::type::value;
        };
    };

    template< typename TypeToSearch >
    struct HasType
    {
        enum { value = FindIndex<TypeToSearch>::value != -1 };
    };

    TYPE_FUNCTION( Get , size_t idx )
    {
        RETURN_TYPE_BY_NAME( type , std::tuple_element<i , std::tuple<T0 , OtherTs...>>::type );
    };

    TYPE_FUNCTION( PushHead , typename pushT )
    {
        RETURN_TYPE_BY_NAME( type , TypeArray<pushT , T0 , OtherTs...> );
    };

    TYPE_FUNCTION( PushTail , typename pushT )
    {
        RETURN_TYPE_BY_NAME( type , TypeArray<T0 , OtherTs... , PushTail> );
    };

    TYPE_FUNCTION( PopHead , typename IgnoreType = void )
    {
        typedef IgnoreType _IgnoreType;
        RETURN_TYPE_BY_NAME( type , TypeArray<OtherTs...> );
    };

    TYPE_FUNCTION( PopTail , typename IgnoreType = void )
    {
        typedef IgnoreType _IgnoreType;
        RETURN_TYPE_BY_NAME( type , details::CutLastElementInParameterPack<TypeArray , T0 , OtherTs...>::type );
    };

};

}

#include "end_mpl_function.h"

#endif // ____TYPEARRAY__74A35F79_37C0_42AA_AF6B_2B2CB336645E