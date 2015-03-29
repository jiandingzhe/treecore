#ifndef TREEJUCE_OBJECT_H
#define TREEJUCE_OBJECT_H

#include "treejuce/Atomic.h"
#include "treejuce/Common.h"
#include "treejuce/LeakedObjectDetector.h"

#include <cstdint>

class TestFramework;

TREEFACE_JUCE_NAMESPACE_BEGIN

class Object
{
    friend class ::TestFramework;
public:
    Object()                    { atomic_fetch_set(&ms_count, 0u); }
    Object(const Object& other) { atomic_fetch_set(&ms_count, 0u); }

    virtual ~Object() {}

    Object& operator = (const Object& other) { return *this; }

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

    JUCE_LEAK_DETECTOR(Object);
}; // class Object

inline void smart_ref(const Object* obj)
{ obj->ref(); }

inline void smart_unref(const Object* obj)
{ obj->unref(); }

TREEFACE_JUCE_NAMESPACE_END

#endif // TREEFACE_OBJECT_H
