#ifndef TREECORE_ATOMIC_TEMPLATE_H
#define TREECORE_ATOMIC_TEMPLATE_H

#include "treecore/Common.h"
#include "treecore/PlatformDefs.h"
#include <cstddef>

namespace treecore {

// get and set
template<typename T>
T atomic_load(const T* store) noexcept;

template<typename T>
void atomic_store(T* store, T value) noexcept;

template<typename T>
void atomic_store(T** store, std::nullptr_t value) noexcept;

template<typename T>
T atomic_exchange(T* store, T value) noexcept;

// get value before modify
template<typename T>
T atomic_fetch_add(T* store, T value) noexcept;
template<typename T>
T atomic_fetch_sub(T* store, T value) noexcept;
template<typename T>
T atomic_fetch_or(T* store, T value) noexcept;
template<typename T>
T atomic_fetch_and(T* store, T value) noexcept;
template<typename T>
T atomic_fetch_xor(T* store, T value) noexcept;
template<typename T>
T atomic_fetch_nand(T* store, T value) noexcept;

// get modified value
template<typename T>
T atomic_add_fetch(T* store, T value) noexcept;
template<typename T>
T atomic_sub_fetch(T* store, T value) noexcept;
template<typename T>
T atomic_or_fetch(T* store, T value) noexcept;
template<typename T>
T atomic_and_fetch(T* store, T value) noexcept;
template<typename T>
T atomic_xor_fetch(T* store, T value) noexcept;
template<typename T>
T atomic_nand_fetch(T* store, T value) noexcept;

// cas
template<typename T>
bool atomic_compare_exchange(T* store, T* expect, T value) noexcept;

template<typename T>
bool atomic_compare_set(T* store, T expect, T value) noexcept;

} // namespace treecore

#endif // TREECORE_ATOMIC_TEMPLATE_H
