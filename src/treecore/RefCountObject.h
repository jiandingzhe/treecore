#ifndef TREECORE_OBJECT_H
#define TREECORE_OBJECT_H

#include "treecore/Atomic.h"
#include "treecore/Common.h"
#include "treecore/LeakedObjectDetector.h"

#include <cstdint>

class TestFramework;

namespace treecore {

class JUCE_API RefCountObject
{
    friend class ::TestFramework;
public:
    RefCountObject()                    { atomic_fetch_set(&ms_count, 0u); }
    RefCountObject(const RefCountObject& other) { atomic_fetch_set(&ms_count, 0u); }

    virtual ~RefCountObject() {}

    RefCountObject& operator = (const RefCountObject& other) { return *this; }

    void ref() const NOEXCEPT
    {
        atomic_fetch_add(&ms_count, 1u);
    }

    void unref() const NOEXCEPT
    {
        if (atomic_fetch_sub(&ms_count, 1u) == 1)
            delete this;
    }

    std::uint32_t get_ref_count() const NOEXCEPT
    {
        return atomic_load(&ms_count);
    }

private:
    mutable std::uint32_t ms_count;

    JUCE_LEAK_DETECTOR(RefCountObject);
}; // class Object

inline void smart_ref(const RefCountObject* obj)
{ obj->ref(); }

inline void smart_unref(const RefCountObject* obj)
{ obj->unref(); }

}

#endif // TREEFACE_OBJECT_H
