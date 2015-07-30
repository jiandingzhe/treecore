#ifndef TREECORE_ARRAY_REF_H
#define TREECORE_ARRAY_REF_H

#include "treecore/Array.h"

namespace treecore {

template<typename T>
class ArrayRef
{
public:
    ArrayRef()
    {
    }

    ArrayRef(T* data, int size)
        : m_data(data)
        , m_size(size)
    {
    }

    ArrayRef(Array<T>& data)
        : m_data(data.getRawDataPointer())
        , m_size(data.size())
    {
    }

    ArrayRef(const ArrayRef& other)
        : m_data(other.m_data)
        , m_size(other.m_size)
    {
    }

    ArrayRef(ArrayRef&& other)
        : m_data(other.m_data)
        , m_size(other.m_size)
    {
        // clearing the other object is probably not necessary
        // just do it for good looking
        other.m_data = nullptr;
        other.m_size = 0;
    }

    ~ArrayRef()
    {
    }

    ArrayRef& operator = (const ArrayRef& other)
    {
        m_data = other.m_data;
        m_size = other.m_size;
        return *this;
    }

    ArrayRef& operator = (ArrayRef&& other)
    {
        m_data = other.m_data;
        m_size = other.m_size;
        // clearing the other object is probably not necessary
        // just do it for good looking
        other.m_data = nullptr;
        other.m_size = 0;
        return *this;
    }

    inline T operator [] (int index) const NOEXCEPT
    {
        return m_data[index];
    }

    inline T& get_reference(int index) NOEXCEPT
    {
        return m_data[index];
    }

    inline const T& get_const_reference(int index) const NOEXCEPT
    {
        return m_data[index];
    }

    inline T* get_data() NOEXCEPT
    {
        return m_data;
    }

    inline const T* get_const_data() const NOEXCEPT
    {
        return m_data;
    }

    inline int size() const NOEXCEPT
    {
        return m_size;
    }

protected:
    T* m_data = nullptr;
    int m_size = 0;
};

}

#endif // TREECORE_ARRAY_REF_H
