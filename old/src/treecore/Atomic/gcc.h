#ifndef TREECORE_ATOMIC_GCC_H
#define TREECORE_ATOMIC_GCC_H

#include "treecore/Atomic/template.h"
#include "treecore/Common.h"

#include "treecore/IntTypes.h"

namespace treecore
{

namespace _impl_
{

template<int SZ>
struct _gcc_atomic_type_helper_;

template<>
struct _gcc_atomic_type_helper_<4>
{
    typedef int32* store_t;
    typedef int32 value_t;
};

template<>
struct _gcc_atomic_type_helper_<8>
{
    typedef int64* store_t;
    typedef int64 value_t;
};

} // namespace _impl_

#define PRI_T        typename _impl_::_gcc_atomic_type_helper_<sizeof(T)>::value_t
#define TO_PRI(arg)  reinterpret_cast<typename _impl_::_gcc_atomic_type_helper_<sizeof(T)>::value_t&>(arg)
#define TO_PRIP(arg) (typename _impl_::_gcc_atomic_type_helper_<sizeof(T)>::store_t)arg
#define TO_ORIG(arg) reinterpret_cast<T&>(arg)

template<typename T>
T atomic_load(const T* store) noexcept
{
    PRI_T re = __atomic_load_n(TO_PRIP(store), __ATOMIC_SEQ_CST);
    return TO_ORIG(re);
}

template<typename T>
void atomic_store(T* store, T value) noexcept
{
    __atomic_store_n(TO_PRIP(store), TO_PRI(value), __ATOMIC_SEQ_CST);
}

template<typename T>
void atomic_store(T** store, std::nullptr_t value) noexcept
{
    __atomic_store_n(TO_PRIP(store), TO_PRI(value), __ATOMIC_SEQ_CST);
}

template<typename T>
T atomic_exchange(T* store, T value) noexcept
{
    PRI_T re = __atomic_exchange_n(TO_PRIP(store), TO_PRI(value), __ATOMIC_SEQ_CST);
    return TO_ORIG(re);
}

template<typename T>
T atomic_fetch_add(T* store, T value) noexcept
{
    PRI_T re = __atomic_fetch_add(TO_PRIP(store), TO_PRI(value), __ATOMIC_SEQ_CST);
    return TO_ORIG(re);
}

template<typename T>
T atomic_fetch_sub(T* store, T value) noexcept
{
    PRI_T re = __atomic_fetch_sub(TO_PRIP(store), TO_PRI(value), __ATOMIC_SEQ_CST);
    return TO_ORIG(re);
}

template<typename T>
T atomic_fetch_or(T* store, T value) noexcept
{
    PRI_T re = __atomic_fetch_or(TO_PRIP(store), TO_PRI(value), __ATOMIC_SEQ_CST);
    return TO_ORIG(re);
}

template<typename T>
T atomic_fetch_and(T* store, T value) noexcept
{
    PRI_T re = __atomic_fetch_and(TO_PRIP(store), TO_PRI(value), __ATOMIC_SEQ_CST);
    return TO_ORIG(re);
}

template<typename T>
T atomic_fetch_xor(T* store, T value) noexcept
{
    PRI_T re = __atomic_fetch_xor(TO_PRIP(store), TO_PRI(value), __ATOMIC_SEQ_CST);
    return TO_ORIG(re);
}

template<typename T>
T atomic_fetch_nand(T* store, T value) noexcept
{
    PRI_T re = __atomic_fetch_nand(TO_PRIP(store), TO_PRI(value), __ATOMIC_SEQ_CST);
    return TO_ORIG(re);
}

template<typename T>
T atomic_add_fetch(T* store, T value) noexcept
{
    PRI_T re = __atomic_add_fetch(TO_PRIP(store), TO_PRI(value), __ATOMIC_SEQ_CST);
    return TO_ORIG(re);
}

template<typename T>
T atomic_sub_fetch(T* store, T value) noexcept
{
    PRI_T re = __atomic_sub_fetch(TO_PRIP(store), TO_PRI(value), __ATOMIC_SEQ_CST);
    return TO_ORIG(re);
}

template<typename T>
T atomic_or_fetch(T* store, T value) noexcept
{
    PRI_T re = __atomic_or_fetch(TO_PRIP(store), TO_PRI(value), __ATOMIC_SEQ_CST);
    return TO_ORIG(re);
}

template<typename T>
T atomic_and_fetch(T* store, T value) noexcept
{
    PRI_T re = __atomic_and_fetch(TO_PRIP(store), TO_PRI(value), __ATOMIC_SEQ_CST);
    return TO_ORIG(re);
}

template<typename T>
T atomic_xor_fetch(T* store, T value) noexcept
{
    PRI_T re = __atomic_xor_fetch(TO_PRIP(store), TO_PRI(value), __ATOMIC_SEQ_CST);
    return TO_ORIG(re);
}

template<typename T>
T atomic_nand_fetch(T* store, T value) noexcept
{
    PRI_T re = __atomic_nand_fetch(TO_PRIP(store), TO_PRI(value), __ATOMIC_SEQ_CST);
    return TO_ORIG(re);
}

template<typename T>
bool atomic_compare_exchange(T* store, T* expect, T value) noexcept
{
    return __atomic_compare_exchange_n(TO_PRIP(store), TO_PRIP(expect), TO_PRI(value), false, __ATOMIC_SEQ_CST, __ATOMIC_SEQ_CST);
}

template<typename T>
bool atomic_compare_set(T* store, T expect, T value) noexcept
{
    return __atomic_compare_exchange_n(TO_PRIP(store), TO_PRIP(&expect), TO_PRI(value), false, __ATOMIC_SEQ_CST, __ATOMIC_SEQ_CST);
}

#undef PRI_T
#undef TO_PRI
#undef TO_PRIP
#undef TO_ORIG

} // namespace treecore


#endif // TREECORE_ATOMIC_GCC_H
