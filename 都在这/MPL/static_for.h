//!********************************************************************************
//! This file is part of the Three-Body Technology code base.
//! Copyright(C), 2012-2015, Three-Body Technology, all rights reserved. 
//! 
//! This file is protected by the copyright law of the people's Republic of China.
//! Permission to use this file is only granted for the employee of Three-Body 
//! Technology, or other company/group/individual who has expressly authorized by
//! Three-Body Technology. 
//!
//! @file    static_for.h
//! @author  Meng Ke
//! @version 1.0
//! @date    2016/02/27
//! @brief   
//!********************************************************************************

#ifndef ____STATIC_FOR__28A85C7F_BE55_4176_B729_5F0B72F8A746
#define ____STATIC_FOR__28A85C7F_BE55_4176_B729_5F0B72F8A746

#include "ValueType.h"
#include "BestParam.h"

namespace treecore {
namespace details {


template< typename FunType , typename CountType , typename... Args >
forcedinline static void staticLoopImpl( const FunType fun , CountType* , Args&&... args ) noexcept
{
    staticLoopImpl<FunType>( fun , ( CountType::decType* )( nullptr ) , args... );
    fun( CountType::value - 1 , args... );
}
template<  typename FunType , typename... Args >
forcedinline static void staticLoopImpl( const FunType , ValueType<0>* , Args&&... ) noexcept
{}

} //namespace details


template< int size , typename FunType , typename... Args >
forcedinline static void staticLoop( const FunType fun , Args&&... args ) noexcept( )
{
    details::staticLoopImpl( fun , ( ValueType<size>* )( nullptr ) , args... );
}


} //namespace treecore


struct K
{
    static forcedinline void runLoop( int counter , int& k )
    {
        k += counter;
    }
};


int main()
{
    int k = 0;
    staticLoop<10>( []( int counter , int& k ) { k += counter; } , k );
    staticLoop<10>( K::runLoop , k );
    system( "PAUSE" );
    return k;
}