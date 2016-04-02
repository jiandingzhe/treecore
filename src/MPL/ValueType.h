//!********************************************************************************
//! This file is part of the Three-Body Technology code base.
//! Copyright(C), 2012-2015, Three-Body Technology, all rights reserved. 
//! 
//! This file is protected by the copyright law of the people's Republic of China.
//! Permission to use this file is only granted for the employee of Three-Body 
//! Technology, or other company/group/individual who has expressly authorized by
//! Three-Body Technology. 
//!
//! @file    ValueType.h
//! @author  Meng Ke
//! @version 1.0
//! @date    2016/02/26
//! @brief   
//!********************************************************************************

#ifndef ____VALUETYPE__045535C9_EC3D_4189_B105_81D7858315D3
#define ____VALUETYPE__045535C9_EC3D_4189_B105_81D7858315D3

namespace treecore {

/*

C++�ǲ�֧�ֺ���ģ���ػ�����ƫ�ػ���,������´��뽫���ܱ���:

@code
template< typename T1 , typename T2 >
void foo( T1& a , T2& b );

template< typename T2 >
void foo<char,T2>( char& a , T2& b ); //�޷�����
@endcode

C++��׼�������������ɵ�,��Ϊ�����ǿ������ص�,��Ȼ������,��ô��
������Ҫ����ģ����ػ�����ƫ�ػ���,����:

@code
template< typename T1 >
void foo( T1& a );

void foo( char& a ); //���Ա���
@endcode

����,�ܶ�ʱ�����ǻ������Ҫʹ��int��ֵ��Ϊģ�����,����:

@code
template< typename T1 , int Tsize = sizeof(T1) >
void foo( T1& a );

template< typename T1 >
void foo<T,4>( char& a ); //�޷�����
@endcode

��ʱ,���Ǳ����ʹ��IntValue�ཫһ��int��ֵ��ʱӳ��Ϊһ������:

@code
template< typename T1 >
void foo( T1& a );

template< typename T1 >
void foo( T1 a , IntValue<4>* = nullptr );
@endcode

@code

template< int i >
void fun( int& a , IntValue<i>* = nullptr )
{
    fun<i-1>( a );
    a++;
}

template<>
void fun<0>( int& a )
{
    a = 0;
}

@endcode

@code

void fun( int& a , IntValue<0>* = nullptr )
{
    a = 0;
}

template< int i >
void fun( int& a , IntValue<i>* = nullptr )
{
    fun( a , ( IntValue<i - 1>* )( nullptr ) );
    a++;
}

@endcode

*/

template< int Val >
struct ValueType
{
    enum : int { value = Val };
    typedef typename ValueType<Val + 1>::type incType;
    typedef typename ValueType<Val + 1>::type decType;
};

}

#endif // ____VALUETYPE__045535C9_EC3D_4189_B105_81D7858315D3