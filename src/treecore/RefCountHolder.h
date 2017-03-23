#ifndef TREECORE_OJBECT_HOLDER_H
#define TREECORE_OJBECT_HOLDER_H

#include "treecore/AtomicObject.h"
#include "treecore/PlatformDefs.h"

class TestFramework;

namespace treecore {

// FIXME test in multiple threads
/**
 *
 */
template<typename T>
class RefCountHolder
{
    friend class ::TestFramework;

    template<typename T1, typename T2>
    friend bool operator == ( const RefCountHolder<T1>& a, const RefCountHolder<T2>& b );

    template<typename T1, typename T2>
    friend bool operator == ( const RefCountHolder<T1>& a, const T2 b );

    template<typename T1, typename T2>
    friend bool operator == ( const T1 a, const RefCountHolder<T2>& b );

    template<typename T1, typename T2>
    friend bool operator != ( const RefCountHolder<T1>& a, const RefCountHolder<T2>& b );

    template<typename T1, typename T2>
    friend bool operator != ( const RefCountHolder<T1>& a, const T2 b );

    template<typename T1, typename T2>
    friend bool operator != ( const T1 a, const RefCountHolder<T2>& b );

    template<typename T1, typename T2>
    friend bool operator < ( const RefCountHolder<T1>& a, const RefCountHolder<T2>& b );

public:
    RefCountHolder() {}

    RefCountHolder( const RefCountHolder& other )
    {
        T* tmp = other.ms_ptr;
        if (tmp)
            smart_ref( tmp );
        ms_ptr = tmp;
    }

    RefCountHolder( RefCountHolder&& other ): ms_ptr( other.ms_ptr )
    {
        other.ms_ptr = nullptr;
    }

    RefCountHolder( T* other )
    {
        if (other)
            smart_ref( other );
        ms_ptr = other;
    }

    ~RefCountHolder()
    {
        if (ms_ptr)
            smart_unref( ms_ptr );
    }

    RefCountHolder& operator = ( const RefCountHolder& other )
    {
        T* this_ptr = ms_ptr;
        T* peer_ptr = other.ms_ptr;

        // first incr then decr, which would prevent incorrect operation when self-assignment
        if (peer_ptr)
            smart_ref( peer_ptr );

        if (this_ptr)
            smart_unref( ms_ptr );

        ms_ptr = peer_ptr;

        return *this;
    }

    RefCountHolder& operator = ( RefCountHolder&& other )
    {
        // When other is destroyed, the swapped out object will unref by one,
        // which is expected.
        swapWith( other );
        return *this;
    }

    RefCountHolder& operator = ( T* peer_ptr )
    {
        T* this_ptr = ms_ptr;

        if (peer_ptr)
            smart_ref( peer_ptr );

        if (this_ptr)
            smart_unref( this_ptr );

        ms_ptr = peer_ptr;

        return *this;
    }

    void swapWith( RefCountHolder& other )
    {
        T* this_ptr = ms_ptr;
        ms_ptr = other.ms_ptr;
        other.ms_ptr = this_ptr;
    }

    operator T* () const noexcept
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

inline void smart_ref( decltype(nullptr) p )
{}

inline void smart_unref( decltype(nullptr) p )
{}

template<typename T1, typename T2>
bool operator == ( const RefCountHolder<T1>& a, const RefCountHolder<T2>& b )
{
    return a.ms_ptr == b.ms_ptr;
}

template<typename T1, typename T2>
bool operator == ( const RefCountHolder<T1>& a, const T2 b )
{
    return a.ms_ptr == b;
}

template<typename T1, typename T2>
bool operator == ( const T1 a, const RefCountHolder<T2>& b )
{
    return a == b.ms_ptr;
}

template<typename T1, typename T2>
bool operator != ( const RefCountHolder<T1>& a, const RefCountHolder<T2>& b )
{
    return a.ms_ptr != b.ms_ptr;
}

template<typename T1, typename T2>
bool operator != ( const RefCountHolder<T1>& a, const T2 b )
{
    return a.ms_ptr != b;
}

template<typename T1, typename T2>
bool operator != ( const T1 a, const RefCountHolder<T2>& b )
{
    return a != b.ms_ptr;
}

template<typename T1, typename T2>
bool operator < ( const RefCountHolder<T1>& a, const RefCountHolder<T2>& b )
{
    return a.ms_ptr < b.ms_ptr;
}

}

#endif // TREECORE_OJBECT_HOLDER_H
