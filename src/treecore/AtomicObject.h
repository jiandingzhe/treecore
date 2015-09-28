#ifndef TREECORE_ATOMIC_OBJECT_H
#define TREECORE_ATOMIC_OBJECT_H

#include "treecore/AtomicFunc.h"

class TestFramework;

namespace treecore
{

template<typename T>
class AtomicObject
{
    friend class ::TestFramework;

public:
    AtomicObject()
    {
        atomic_store<T>(&m_data, 0);
    }

    AtomicObject(T value)
    {
        atomic_store<T>(&m_data, value);
    }

    ~AtomicObject() {}

    T* get_raw() noexcept
    {
        return &m_data;
    }

    T load() noexcept
    {
        return atomic_load<T>(&m_data);
    }

    operator T ()
    {
        return atomic_load<T>(&m_data);
    }

    void store(T value) noexcept
    {
        atomic_store<T>(&m_data, value);
    }

    T operator = (T value) noexcept
    {
        atomic_store<T>(&m_data, value);
        return value;
    }

    T exchange(T value) noexcept
    {
        return atomic_exchange<T>(&m_data, value);
    }

    T operator ++ () noexcept
    {
        return atomic_add_fetch<T>(&m_data, 1);
    }

    T operator -- () noexcept
    {
        return atomic_sub_fetch<T>(&m_data, 1);
    }

    T operator ++ (int) noexcept
    {
        return atomic_fetch_add<T>(&m_data, 1);
    }

    T operator -- (int) noexcept
    {
        return atomic_fetch_sub<T>(&m_data, 1);
    }

    T operator += (T value) noexcept
    {
        return atomic_add_fetch<T>(&m_data, value);
    }

    T operator -= (T value) noexcept
    {
        return atomic_sub_fetch<T>(&m_data, value);
    }

    T operator &= (T value) noexcept
    {
        return atomic_and_fetch<T>(&m_data, value);
    }

    T operator |= (T value) noexcept
    {
        return atomic_or_fetch<T>(&m_data, value);
    }

    T operator ^= (T value) noexcept
    {
        return atomic_xor_fetch<T>(&m_data, value);
    }

    /**
     * @brief performs CAS operation
     * @param expect contains expected value. If CAS is not taken, current value is extracted and stored here.
     * @param value if current == expect, value is written
     * @return whether current == expect
     */
    bool compare_exchange(T* expect, T value) noexcept
    {
        return atomic_compare_exchange<T>(&m_data, expect, value);
    }

    bool compare_set(T expect, T value) noexcept
    {
        return atomic_compare_set<T>(&m_data, expect, value);
    }

private:
    T m_data;
};

} // namespace treecore

#endif // TREECORE_ATOMIC_OBJECT_H
