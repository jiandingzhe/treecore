#ifndef TREEFACE_OJBECT_HOLDER_H
#define TREEFACE_OJBECT_HOLDER_H

#include "treecore/Common.h"
#include "treecore/PlatformDefs.h"

class TestFramework;

namespace treecore {

template<typename T>
class RefCountHolder
{
    friend class ::TestFramework;

    template<typename T1, typename T2>
    friend bool operator == (const RefCountHolder<T1>& a, const RefCountHolder<T2>& b);

    template<typename T1, typename T2>
    friend bool operator == (const RefCountHolder<T1>& a, const T2 b);

    template<typename T1, typename T2>
    friend bool operator == (const T1 a, const RefCountHolder<T2>& b);

    template<typename T1, typename T2>
    friend bool operator != (const RefCountHolder<T1>& a, const RefCountHolder<T2>& b);

    template<typename T1, typename T2>
    friend bool operator != (const RefCountHolder<T1>& a, const T2 b);

    template<typename T1, typename T2>
    friend bool operator != (const T1 a, const RefCountHolder<T2>& b);

    template<typename T1, typename T2>
    friend bool operator < (const RefCountHolder<T1>& a, const RefCountHolder<T2>& b);

public:
    RefCountHolder() {}

    RefCountHolder(const RefCountHolder& other): ms_ptr(other.ms_ptr)
    {
        if (ms_ptr)
            smart_ref(ms_ptr);
    }

    RefCountHolder(RefCountHolder&& other): ms_ptr(other.ms_ptr)
    {
        other.ms_ptr = nullptr;
    }

    RefCountHolder(T* other): ms_ptr(other)
    {
        if (ms_ptr)
            smart_ref(ms_ptr);
    }

    ~RefCountHolder()
    {
        if (ms_ptr)
            smart_unref(ms_ptr);
    }

    RefCountHolder& operator = (const RefCountHolder& other)
    {
        if (ms_ptr)
            smart_unref(ms_ptr);

        ms_ptr = other.ms_ptr;

        if (ms_ptr)
            smart_ref(ms_ptr);

        return *this;
    }

    RefCountHolder& operator = (RefCountHolder&& other)
    {
        T* tmp = ms_ptr;
        ms_ptr = other.ms_ptr;
        other.ms_ptr = tmp;

        return *this;
    }

    RefCountHolder& operator = (T* other)
    {
        if (ms_ptr)
            smart_unref(ms_ptr);

        ms_ptr = other;

        if(ms_ptr)
            smart_ref(ms_ptr);

        return *this;
    }

    operator T*() const noexcept
    {
        return ms_ptr;
    }

    operator bool () const noexcept
    {
        return ms_ptr != nullptr;
    }

    T* get() const noexcept
    {
        return ms_ptr;
    }

    T& operator * () const noexcept
    {
        return *ms_ptr;
    }

    T* operator -> () const noexcept
    {
        return ms_ptr;
    }

private:
    T* ms_ptr = nullptr;
}; // class ObjectHolder

inline void smart_ref(decltype(nullptr) p)
{
}

inline void smart_unref(decltype(nullptr) p)
{
}

template<typename T1, typename T2>
bool operator == (const RefCountHolder<T1>& a, const RefCountHolder<T2>& b)
{
    return a.ms_ptr == b.ms_ptr;
}

template<typename T1, typename T2>
bool operator == (const RefCountHolder<T1>& a, const T2 b)
{
    return a.ms_ptr == b;
}

template<typename T1, typename T2>
bool operator == (const T1 a, const RefCountHolder<T2>& b)
{
    return a == b.ms_ptr;
}

template<typename T1, typename T2>
bool operator != (const RefCountHolder<T1>& a, const RefCountHolder<T2>& b)
{
    return a.ms_ptr != b.ms_ptr;
}

template<typename T1, typename T2>
bool operator != (const RefCountHolder<T1>& a, const T2 b)
{
    return a.ms_ptr != b;
}

template<typename T1, typename T2>
bool operator != (const T1 a, const RefCountHolder<T2>& b)
{
    return a != b.ms_ptr;
}


template<typename T1, typename T2>
bool operator < (const RefCountHolder<T1>& a, const RefCountHolder<T2>& b)
{
    return a.ms_ptr < b.ms_ptr;
}

}

#endif // TREEFACE_OJBECT_HOLDER_H
