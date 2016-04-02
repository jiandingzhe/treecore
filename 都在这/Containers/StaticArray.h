//
//  main.cpp
//  kkkk
//
//  Created by MengKe on 16/1/2.
//  Copyright © 2016年 MengKe. All rights reserved.
//

#include <iostream>
#include <array>

//-----------------------------------------------------------------

template < typename T , int alignSize = alignof(T) >
class AlignMark
{
public:
    typedef T type;
    enum { value = alignSize };
};

template < typename T >
class AlignMarkUppack
{
public:
    typedef T type;
    enum { value = alignof(T) };
};

template < typename T, int alignSize >
class AlignMarkUppack<AlignMark<T,alignSize>>
{
public:
    typedef T type;
    enum { value = alignSize };
};

//-----------------------------------------------------------------

template< typename T, size_t... dimSizes >
class StaticArray
{};

template< typename T, size_t dim0Size, size_t... otherDimSizes >
class StaticArray<T,dim0Size,otherDimSizes...>
{
public:
    typedef Array<T,otherDimSizes...> ChildArray;
    typedef std::initializer_list<typename ChildArray::InitListType> InitListType;
    
    StaticArray()=default;

    StaticArray( const T* ptr , int num )
    {

    }

    StaticArray( const StaticArray& other )
    {
        int i = 0;
        for( e : *this ) e = other[i++];
    }
    
    StaticArray( InitListType initList )
    {
        if( initList.size() == 1 )
        {

        }
    }

    StaticArray& operator= ( InitListType initList )
    {
        if( initList.size() == 1 )
        {
            for( e : *this ) e = initList.begin()[0];
        }
        else
        {
            const ChildArray::InitListType* k = initList.begin();
            for( e : *this ) e = *( k++ );
        }
    }

    template< typename CopyType >
    StaticArray& setAll( const BestParam<CopyType> data )
    {

    }

    template< typename CopyType >
    StaticArray& set( const CopyType* ptr , int num )
    {

    }
    
    template< typename IdxType >
    forcedinline ChildArray& operator[]( const IdxType i ) noexcept
    {
        const ssize_t idx = (ssize_t)i;
        const ssize_t theSize = (ssize_t)dim0Size;
        tassert( idx >= 0 && idx < theSize );
        (void)idx;
        (void)theSize;
        return data[i];
    }
    
    template< typename IdxType >
    forcedinline const ChildArray& operator[]( const IdxType i ) const noexcept
    {
        const ssize_t idx = (ssize_t)i;
        const ssize_t theSize = (ssize_t)dim0Size;
        tassert( idx >= 0 && idx < theSize );
        (void)idx;
        (void)theSize;
        return data[i];
    }

    forcedinline T* begin() noexcept { return &data[0]; }

    forcedinline const T* end() consst noexcept { return &data[dim0Size]; }
    
private:
    ChildArray data[dim0Size];
};


template< typename T, size_t dim0Size >
class StaticArray<T,dim0Size>
{
public:
    typedef typename AlignMarkUppack<T>::type ValueType;
    enum { alignSize = AlignMarkUppack<T>::value };
    typedef ValueType ChildArray;
    typedef std::initializer_list<ValueType> InitListType;
    Array()=default;
    Array( InitListType initList ){};
};

template < typename T >
class StaticArray<T>
{
public:
    
};


template < typename T >
union Data
{
    template< typename... Args >
    Data( std::initializer_list<Args>... args )
    {
        for( int i=0; i<100; ++i ) {
            //new(r[i]) T( args...);
        }
    };
    
    template< typename... Args >
    Data( const Args&&... args )
    {
        for( int i=0; i<100; ++i ) {
            new(r[i]) T(args...);
        }
    };
    
    Data()
    {
        for( int i=0; i<100; ++i ) {
            new(r[i]) T();
        }
    };
    
    ~Data()
    {
        for( int i=0; i<100; ++i ){
            (v+i)->~T();
        }
    }
    
    T v[100];
    char r[100][sizeof(T)];
};



class AA
{
public:
    AA( int i , float k):I(i),K(k){};
    AA(const AA& ){};
    ~AA(){};
private:
    int I;
    float K;
};

int main()
{
    AA u(2,0);
    Data<AA> k={{ 0,1,5,6,7 }};
    
    Array<AlignMark<int,16>,2,2> arr={ {1,1},{2,2} };
}

