#ifndef TREECORE_SIMD_OBJECT_H
#define TREECORE_SIMD_OBJECT_H

#include "treecore/SimdFunc.h"
#include "treecore/AlignedMalloc.h"
#include "treecore/IntUtils.h"

namespace treecore
{

template<typename T, int N_ELEM>
struct SimdObject
{
    typedef SIMDType<sizeof(T)* N_ELEM> DataType;
    typedef typename similar_int<T>::type IntType;

    TREECORE_ALIGNED_ALLOCATOR( SimdObject )

    SimdObject() noexcept
    {
        simd_set_all<T>( data, 0, 0, 0, 0 );
    }

    SimdObject( const SimdObject& peer ) noexcept
        : data( peer.data )
    {}

    explicit SimdObject( const DataType& value ) noexcept
        : data( value )
    {}

    explicit SimdObject( T value ) noexcept
    {
        simd_broadcast<T>( data, value );
    }

    SimdObject( const T* value_ptr ) noexcept
    {
        simd_set_all( data, value_ptr );
    }

    SimdObject( T v1, T v2 ) noexcept
    {
        simd_set_all<T>( data, v1, v2 );
    }

    SimdObject( T v1, T v2, T v3, T v4 ) noexcept
    {
        simd_set_all<T>( data, v1, v2, v3, v4 );
    }

    SimdObject( T v1, T v2, T v3, T v4, T v5, T v6, T v7, T v8 ) noexcept
    {
        simd_set_all<T>( data, v1, v2, v3, v4, v5, v6, v7, v8 );
    }

    template<typename T2, int N_ELEM2>
    operator SimdObject<T2, N_ELEM2>() { return data; }

    template<typename T2>
    SimdObject<T2, N_ELEM> convert_to()
    {
        SimdObject<T2, N_ELEM> result;
        simd_convert<T2, T>( result.data, data );
        return result;
    }

    template<int IDX> T get() const noexcept
    {
        return simd_get_one<IDX, T>( data );
    }

    template<int IDX> void set( T value ) noexcept
    {
        simd_set_one<IDX, T>( data, value );
    }

    void set_all( T v1, T v2 ) noexcept
    {
        simd_set_all<T>( data, v1, v2 );
    }

    void set_all( T v1, T v2, T v3, T v4 ) noexcept
    {
        simd_set_all<T>( data, v1, v2, v3, v4 );
    }

    void set_all( T v1, T v2, T v3, T v4, T v5, T v6, T v7, T v8 ) noexcept
    {
        simd_set_all<T>( data, v1, v2, v3, v4, v5, v6, v7, v8 );
    }

    void set_all( const T* values ) noexcept
    {
        simd_set_all<T>( data, values );
    }

    void get_all( T* store ) noexcept
    {
        simd_get_all<T>( data, store );
    }

    SimdObject& operator = ( const SimdObject& peer ) noexcept
    {
        data = peer.data;
        return *this;
    }

    SimdObject& operator = ( const DataType& value ) noexcept
    {
        data = value;
        return *this;
    }

    SimdObject& operator = ( T value ) noexcept
    {
        simd_broadcast( data, value );
        return *this;
    }

    SimdObject operator + ( const SimdObject& peer ) const noexcept
    {
        SimdObject result;
        simd_add<T>( result.data, data, peer.data );
        return result;
    }

    SimdObject operator - ( const SimdObject& peer ) const noexcept
    {
        SimdObject result;
        simd_sub<T>( result.data, data, peer.data );
        return result;
    }

    SimdObject operator * ( const SimdObject& peer ) const noexcept
    {
        SimdObject result;
        simd_mul<T>( result.data, data, peer.data );
        return result;
    }

    SimdObject operator / ( const SimdObject& peer ) const noexcept
    {
        SimdObject result;
        simd_div<T>( result.data, data, peer.data );
        return result;
    }

    template<typename T2, int N_ELEM2>
    SimdObject operator & ( const SimdObject<T2, N_ELEM2>& peer ) const noexcept
    {
        SimdObject result;
        simd_and<T>( result.data, data, peer.data );
        return result;
    }

    template<typename T2, int N_ELEM2>
    SimdObject operator | ( const SimdObject<T2, N_ELEM2>& peer ) const noexcept
    {
        SimdObject result;
        simd_or<T>( result.data, data, peer.data );
        return result;
    }

    template<typename T2, int N_ELEM2>
    SimdObject operator ^ ( const SimdObject<T2, N_ELEM2>& peer ) const noexcept
    {
        SimdObject result;
        simd_xor<T>( result.data, data, peer.data );
        return result;
    }

    SimdObject<T, N_ELEM> operator << ( int shift_num ) const noexcept
    {
        SimdObject result;
        simd_shift_left( result.data, data, shift_num );
        return result;
    }

    SimdObject<T, N_ELEM> operator >> ( int shift_num ) const noexcept
    {
        SimdObject result;
        simd_shift_right_zero( result.data, data, shift_num );
        return result;
    }

    SimdObject& operator += ( const SimdObject& peer ) noexcept
    {
        simd_add<T>( data, data, peer.data );
        return *this;
    }

    SimdObject& operator -= ( const SimdObject& peer ) noexcept
    {
        simd_sub<T>( data, data, peer.data );
        return *this;
    }

    SimdObject& operator *= ( const SimdObject& peer ) noexcept
    {
        simd_mul<T>( data, data, peer.data );
        return *this;
    }

    SimdObject& operator /= ( const SimdObject& peer ) noexcept
    {
        simd_div<T>( data, data, peer.data );
        return *this;
    }

    template<typename T2, int N_ELEM2>
    SimdObject& operator &= ( const SimdObject<T2, N_ELEM2>& peer ) noexcept
    {
        simd_and<T>( data, data, peer.data );
        return *this;
    }

    template<typename T2, int N_ELEM2>
    SimdObject& operator |= ( const SimdObject<T2, N_ELEM2>& peer ) noexcept
    {
        simd_or<T>( data, data, peer.data );
        return *this;
    }

    template<typename T2, int N_ELEM2>
    SimdObject& operator ^= ( const SimdObject<T2, N_ELEM2>& peer ) noexcept
    {
        simd_xor<T>( data, data, peer.data );
        return *this;
    }

    SimdObject operator ~ () const noexcept
    {
        SimdObject result( data );
        simd_cmpl<T>( result.data );
        return result;
    }

    SimdObject<IntType, N_ELEM> operator == ( const SimdObject& peer ) const noexcept
    {
        SimdObject<IntType, N_ELEM> result;
        simd_cmp<T>( result.data, data, peer.data );
        return result;
    }

    SimdObject<IntType, N_ELEM> operator != ( const SimdObject& peer ) const noexcept
    {
        SimdObject<IntType, N_ELEM> result;
        simd_ncmp<T>( result.data, data, peer.data );
        return result;
    }

    SimdObject<IntType, N_ELEM> operator > ( const SimdObject& peer ) const noexcept
    {
        SimdObject<IntType, N_ELEM> result;
        simd_gt<T>( result.data, data, peer.data );
        return result;
    }

    SimdObject<IntType, N_ELEM> operator >= ( const SimdObject& peer ) const noexcept
    {
        SimdObject<IntType, N_ELEM> result;
        simd_ge<T>( result.data, data, peer.data );
        return result;
    }

    SimdObject<IntType, N_ELEM> operator < ( const SimdObject& peer ) const noexcept
    {
        SimdObject<IntType, N_ELEM> result;
        simd_lt<T>( result.data, data, peer.data );
        return result;
    }

    SimdObject<IntType, N_ELEM> operator <= ( const SimdObject& peer ) const noexcept
    {
        SimdObject<IntType, N_ELEM> result;
        simd_le<T>( result.data, data, peer.data );
        return result;
    }

    template<int IDX1, int IDX2, int IDX3, int IDX4>
    void shuffle() noexcept
    {
        simd_shuffle<IDX1, IDX2, IDX3, IDX4>( data );
    }

    template<int IDX1, int IDX2, int IDX3, int IDX4>
    SimdObject get_shuffle() const noexcept
    {
        SimdObject result( *this );
        result.template shuffle<IDX1, IDX2, IDX3, IDX4>();
        return result;
    }

    T sum() const noexcept
    {
        return simd_sum<T>( data );
    }

    DataType data;
};

typedef SimdObject<float, 4> simd4f;
typedef SimdObject<int32, 4> simd4i;

} // namespace treecore

#endif // TREECORE_SIMD_OBJECT_H
