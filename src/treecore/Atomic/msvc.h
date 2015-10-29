#ifndef TREECORE_ATOMIC_MSVC_H
#define TREECORE_ATOMIC_MSVC_H

#include "treecore/Atomic/template.h"

#include <intrin.h>
#include <cstdlib>

namespace treecore
{

namespace _impl_
{


#define TO_PRI(arg)  reinterpret_cast<value_type&>(arg)
#define TO_PRIP(arg) store_type(arg)
#define TO_ORIG(arg) reinterpret_cast<T&>(arg)

template<typename T, int SZ>
struct _msvc_atomic_impl_ {};

template<typename T>
struct _msvc_atomic_impl_<T, 4>
{
    typedef long volatile* store_type;
    typedef long value_type;

    static T load(const T* store) noexcept
    {
        value_type re = _InterlockedExchangeAdd( TO_PRIP(store), 0 );
        return TO_ORIG(re);
    }

    static void store(T* store, T value) noexcept
    {
        _InterlockedExchange( TO_PRIP(store), TO_PRI(value) );
    }

    static T fetch_set(T* store, T value) noexcept
    {
        value_type re = _InterlockedExchange( TO_PRIP(store), TO_PRI(value) );
        return TO_ORIG(re);
    }

    // get value before modify

    static T fetch_add(T* store, T value) noexcept
    {
        value_type re = _InterlockedExchangeAdd( TO_PRIP(store), TO_PRI(value) );
        return TO_ORIG(re);
    }

    static T fetch_sub(T* store, T value) noexcept
    {
        value_type re = _InterlockedExchangeAdd( TO_PRIP(store), -TO_PRI(value) );
        return TO_ORIG(re);
    }

    static T fetch_or(T* store, T value) noexcept
    {
        value_type re = _InterlockedOr( TO_PRIP(store), TO_PRI(value) );
        return TO_ORIG(re);
    }

    static T fetch_and(T* store, T value) noexcept
    {
        value_type re = _InterlockedAnd( TO_PRIP(store), TO_PRI(value) );
        return TO_ORIG(re);
    }

    static T fetch_xor(T* store, T value) noexcept
    {
        value_type re = _InterlockedXor( TO_PRIP(store), TO_PRI(value) );
        return TO_ORIG(re);
    }

    // get modified value
    static T add_fetch(T* store, T value) noexcept
    {
        value_type tmp = _InterlockedExchangeAdd(TO_PRIP(store), TO_PRI(value));
        tmp += TO_PRI(value);
        return TO_ORIG(tmp);
    }

    static T sub_fetch(T* store, T value) noexcept
    {
        value_type tmp = _InterlockedExchangeAdd(TO_PRIP(store), -TO_PRI(value));
        tmp -= TO_PRI(value);
        return TO_ORIG(tmp);
    }

    static T or_fetch(T* store, T value) noexcept
    {
        value_type tmp = _InterlockedOr(TO_PRIP(store), TO_PRI(value));
        tmp |= TO_PRI(value);
        return TO_ORIG(tmp);
    }

    static T and_fetch(T* store, T value) noexcept
    {
        value_type tmp = _InterlockedAnd(TO_PRIP(store), TO_PRI(value));
        tmp &= TO_PRI(value);
        return TO_ORIG(tmp);
    }

    static T xor_fetch(T* store, T value) noexcept
    {
        value_type tmp = _InterlockedXor(TO_PRIP(store), TO_PRI(value));
        tmp ^= TO_PRI(value);
        return TO_ORIG(tmp);
    }

    // CAS
    static bool cas(T* store, T expect, T value) noexcept
    {
        value_type old = _InterlockedCompareExchange(TO_PRIP(store), TO_PRI(value), TO_PRI(expect));
        return old == TO_PRI(expect);
    }

    static bool cae(T* store, T* expect, T value) noexcept
    {
        value_type old = _InterlockedCompareExchange(TO_PRIP(store), TO_PRI(value), TO_PRI(*expect));
        bool re = (old == TO_PRI(*expect));
        *expect = TO_ORIG(old);
        return re;
    }
};

template<typename T>
struct _msvc_atomic_impl_<T, 8>
{
    typedef __int64 volatile* store_type;
    typedef __int64 value_type;

    static T load(const T* store) noexcept
    {
        value_type re = _InterlockedExchangeAdd64( TO_PRIP(store), 0 );
        return TO_ORIG(re);
    }

    static void store(T* store, T value) noexcept
    {
        _InterlockedExchange64( TO_PRIP(store), TO_PRI(value) );
    }

    static T fetch_set(T* store, T value) noexcept
    {
        value_type re = _InterlockedExchange64( TO_PRIP(store), TO_PRI(value) );
        return TO_ORIG(re);
    }

    // get value before modify
    static T fetch_add(T* store, T value) noexcept
    {
        value_type re = _InterlockedExchangeAdd64( TO_PRIP(store), TO_PRI(value) );
        return TO_ORIG(re);
    }

    static T fetch_sub(T* store, T value) noexcept
    {
        value_type re = _InterlockedExchangeAdd64( TO_PRIP(store), -TO_PRI(value) );
        return TO_ORIG(re);
    }

    static T fetch_or(T* store, T value) noexcept
    {
        value_type re = _InterlockedOr64( TO_PRIP(store), TO_PRI(value) );
        return TO_ORIG(re);
    }

    static T fetch_and(T* store, T value) noexcept
    {
        value_type re = _InterlockedAnd64( TO_PRIP(store), TO_PRI(value) );
        return TO_ORIG(re);
    }

    static T fetch_xor(T* store, T value) noexcept
    {
        value_type re = _InterlockedXor64( TO_PRIP(store), TO_PRI(value) );
        return TO_ORIG(re);
    }

    // get modified value
    static T add_fetch(T* store, T value) noexcept
    {
        value_type tmp = _InterlockedExchangeAdd64( TO_PRIP(store), TO_PRI(value) );
        tmp += TO_PRI(value);
        return TO_ORIG(tmp);
    }

    static T sub_fetch(T* store, T value) noexcept
    {
        value_type tmp = _InterlockedExchangeAdd64(TO_PRIP(store), -TO_PRI(value));
        tmp -= TO_PRI(value);
        return TO_ORIG(tmp);
    }

    static T or_fetch(T* store, T value) noexcept
    {
        value_type tmp = _InterlockedOr64(TO_PRIP(store), TO_PRI(value));
        tmp |= TO_PRI(value);
        return TO_ORIG(tmp);
    }

    static T and_fetch(T* store, T value) noexcept
    {
        value_type tmp = _InterlockedAnd64(TO_PRIP(store), TO_PRI(value));
        tmp &= TO_PRI(value);
        return TO_ORIG(tmp);
    }

    static T xor_fetch(T* store, T value) noexcept
    {
        value_type tmp = _InterlockedXor64(TO_PRIP(store), TO_PRI(value));
        tmp ^= TO_PRI(value);
        return TO_ORIG(tmp);
    }

    // CAS
    static bool cas(T* store, T expect, T value) noexcept
    {
        value_type old = _InterlockedCompareExchange64(TO_PRIP(store), TO_PRI(value), TO_PRI(expect));
        return old == TO_PRI(expect);
    }

    static bool cae(T* store, T* expect, T value) noexcept
    {
        value_type old = _InterlockedCompareExchange64( TO_PRIP(store), TO_PRI(value), TO_PRI(*expect) );
        bool re = (old == TO_PRI(*expect));
        *expect = TO_ORIG(old);
        return re;
    }
};

#undef TO_PRI
#undef TO_PRIP
#undef TO_ORIG

} // namespace _impl_

// get and set
template<typename T>
inline T atomic_load(const T* store) noexcept
{
    return _impl_::_msvc_atomic_impl_<T, sizeof(T)>::load(store);
}

template<typename T>
void atomic_store(T* store, T value) noexcept
{
    _impl_::_msvc_atomic_impl_<T, sizeof(T)>::store(store, value);
}

template<typename T>
T atomic_exchange(T* store, T value) noexcept
{
    return _impl_::_msvc_atomic_impl_<T, sizeof(T)>::fetch_set(store, value);
}

// get value before modify
template<typename T>
inline T atomic_fetch_add(T* store, T value) noexcept
{
    return _impl_::_msvc_atomic_impl_<T, sizeof(T)>::fetch_add(store, value);
}

template<typename T>
inline T atomic_fetch_sub(T* store, T value) noexcept
{
    return _impl_::_msvc_atomic_impl_<T, sizeof(T)>::fetch_sub(store, value);
}

template<typename T>
inline T atomic_fetch_or(T* store, T value) noexcept
{
    return _impl_::_msvc_atomic_impl_<T, sizeof(T)>::fetch_or(store, value);
}

template<typename T>
inline T atomic_fetch_and(T* store, T value) noexcept
{
    return _impl_::_msvc_atomic_impl_<T, sizeof(T)>::fetch_and(store, value);
}

template<typename T>
inline T atomic_fetch_xor(T* store, T value) noexcept
{
    return _impl_::_msvc_atomic_impl_<T, sizeof(T)>::fetch_xor(store, value);
}

template<typename T>
inline T atomic_fetch_nand(T* store, T value) noexcept
{
    return _impl_::_msvc_atomic_impl_<T, sizeof(T)>::fetch_nand(store, value);
}

// get modified value
template<typename T>
inline T atomic_add_fetch(T* store, T value) noexcept
{
    return _impl_::_msvc_atomic_impl_<T, sizeof(T)>::add_fetch(store, value);
}

template<typename T>
inline T atomic_sub_fetch(T* store, T value) noexcept
{
    return _impl_::_msvc_atomic_impl_<T, sizeof(T)>::sub_fetch(store, value);
}

template<typename T>
inline T atomic_or_fetch(T* store, T value) noexcept
{
    return _impl_::_msvc_atomic_impl_<T, sizeof(T)>::or_fetch(store, value);
}

template<typename T>
inline T atomic_and_fetch(T* store, T value) noexcept
{
    return _impl_::_msvc_atomic_impl_<T, sizeof(T)>::and_fetch(store, value);
}

template<typename T>
inline T atomic_xor_fetch(T* store, T value) noexcept
{
    return _impl_::_msvc_atomic_impl_<T, sizeof(T)>::xor_fetch(store, value);
}

// cas
template<typename T>
inline bool atomic_compare_exchange(T* store, T* expect, T value) noexcept
{
    return _impl_::_msvc_atomic_impl_<T, sizeof(T)>::cae(store, expect, value);
}

template<typename T>
inline bool atomic_compare_set(T* store, T expect, T value) noexcept
{
    return _impl_::_msvc_atomic_impl_<T, sizeof(T)>::cas(store, expect, value);
}

} // namespace treecore


#endif // TREECORE_ATOMIC_MSVC_H
