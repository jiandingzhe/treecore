#ifndef TREECORE_SIMD_OBJECT_H
#define TREECORE_SIMD_OBJECT_H

#include "treecore/SimdFunc.h"
#include "treecore/AlignedMalloc.h"

namespace treecore
{

template<typename T, int N_ELEM>
struct SimdObject
{
    typedef SIMDType<sizeof(T) * N_ELEM> DataType;

    TREECORE_ALIGNED_ALLOCATOR(SimdObject);

    SimdObject() noexcept
    {
        simd_set_all<T>(data, 0, 0, 0, 0);
    }

    SimdObject(const SimdObject& peer) noexcept
        : data(peer.data)
    {
    }

    explicit SimdObject(const DataType& value) noexcept
        : data(value)
    {
    }

    explicit SimdObject(T value) noexcept
    {
        simd_broadcast<T>(data, value);
    }

    SimdObject(T v1, T v2) noexcept
    {
        simd_set_all<T>(data, v1, v2);
    }

    SimdObject(T v1, T v2, T v3, T v4) noexcept
    {
        simd_set_all<T>(data, v1, v2, v3, v4);
    }

    SimdObject(T v1, T v2, T v3, T v4, T v5, T v6, T v7, T v8) noexcept
    {
        simd_set_all<T>(data, v1, v2, v3, v4);
    }

    template<int IDX> T get() const noexcept
    {
        return simd_get_one<IDX, T>(data);
    }

    template<int IDX> void set(T value) noexcept
    {
        simd_set_one<IDX, T>(data, value);
    }

    void set_all(T v1, T v2) noexcept
    {
        simd_set_all<T>(data, v1, v2);
    }

    void set_all(T v1, T v2, T v3, T v4) noexcept
    {
        simd_set_all<T>(data, v1, v2, v3, v4);
    }

    void set_all(T v1, T v2, T v3, T v4, T v5, T v6, T v7, T v8) noexcept
    {
        simd_set_all<T>(data, v1, v2, v3, v4, v5, v6, v7, v8);
    }

    SimdObject& operator = (const SimdObject& peer) noexcept
    {
        data = peer.data;
        return *this;
    }

    SimdObject& operator = (T value) noexcept
    {
        simd_broadcast(data, value);
        return *this;
    }

    SimdObject operator + (const SimdObject& peer) const noexcept
    {
        SimdObject result;
        simd_add<T>(result.data, data, peer.data);
        return result;
    }

    SimdObject operator - (const SimdObject& peer) const noexcept
    {
        SimdObject result;
        simd_sub<T>(result.data, data, peer.data);
        return result;
    }

    SimdObject operator * (const SimdObject& peer) const noexcept
    {
        SimdObject result;
        simd_mul<T>(result.data, data, peer.data);
        return result;
    }

    SimdObject operator / (const SimdObject& peer) const noexcept
    {
        SimdObject result;
        simd_div<T>(result.data, data, peer.data);
        return result;
    }

    template<typename T2, int N_ELEM2>
    SimdObject operator & (const SimdObject<T2, N_ELEM2>& peer) const noexcept
    {
        SimdObject result;
        simd_and<T>(result.data, data, peer.data);
        return result;
    }

    template<typename T2, int N_ELEM2>
    SimdObject operator | (const SimdObject<T2, N_ELEM2>& peer) const noexcept
    {
        SimdObject result;
        simd_or<T>(result.data, data, peer.data);
        return result;
    }

    template<typename T2, int N_ELEM2>
    SimdObject operator ^ (const SimdObject<T2, N_ELEM2>& peer) const noexcept
    {
        SimdObject result;
        simd_xor<T>(result.data, data, peer.data);
        return result;
    }

    SimdObject& operator += (const SimdObject& peer) noexcept
    {
        simd_add<T>(data, data, peer.data);
        return *this;
    }

    SimdObject& operator -= (const SimdObject& peer) noexcept
    {
        simd_sub<T>(data, data, peer.data);
        return *this;
    }

    SimdObject& operator *= (const SimdObject& peer) noexcept
    {
        simd_mul<T>(data, data, peer.data);
        return *this;
    }

    SimdObject& operator /= (const SimdObject& peer) noexcept
    {
        simd_div<T>(data, data, peer.data);
        return *this;
    }

    template<typename T2, int N_ELEM2>
    SimdObject& operator &= (const SimdObject<T2, N_ELEM2>& peer) noexcept
    {
        simd_and<T>(data, data, peer.data);
        return *this;
    }

    template<typename T2, int N_ELEM2>
    SimdObject& operator |= (const SimdObject<T2, N_ELEM2>& peer) noexcept
    {
        simd_or<T>(data, data, peer.data);
        return *this;
    }

    template<typename T2, int N_ELEM2>
    SimdObject& operator ^= (const SimdObject<T2, N_ELEM2>& peer) noexcept
    {
        simd_xor<T>(data, data, peer.data);
        return *this;
    }

    SimdObject operator ~ () const noexcept
    {
        SimdObject result(data);
        simd_cmpl<T>(result.data);
        return result;
    }

    template<int IDX1, int IDX2, int IDX3, int IDX4>
    void shuffle() noexcept
    {
        simd_shuffle<IDX1, IDX2, IDX3, IDX4>(data);
    }

    T sum() const noexcept
    {
        return simd_sum<T>(data);
    }

    DataType data;
};

} // namespace treecore

#endif // TREECORE_SIMD_OBJECT_H
