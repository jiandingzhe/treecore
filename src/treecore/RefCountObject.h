#ifndef TREECORE_OBJECT_H
#define TREECORE_OBJECT_H

#include "treecore/AtomicObject.h"
#include "treecore/Common.h"
#include "treecore/IntTypes.h"
#include "treecore/LeakedObjectDetector.h"

class TestFramework;

namespace treecore {

/**
 * @brief base class of reference-counted objects
 *
 * Contains a 32-bit reference counter, and will destruct itself when reference
 * count drops to zero. The counter is atomic operated so that thread safety is
 * ensured.
 *
 * The reference count is initially zero after construction, so you should
 * increase it immediately if you want to hold its ownership.
 *
 * The header file already provides smart_ref() and smart_unref functions, so
 * that RefCountObject can be directly used by RefCountHolder, which
 * automatically handles reference count at construction and destruction.
 */
class TREECORE_SHARED_API RefCountObject
{
    friend class ::TestFramework;
public:
    /**
     * @brief reference count will be initially zero
     */
    RefCountObject()
    {
        ms_count.store(0);
    }

    /**
     * @brief reference count will be initially zero
     * @param other
     */
    RefCountObject(const RefCountObject& other)
    {
        ms_count.store(0);
    }

    virtual ~RefCountObject() {}

    RefCountObject& operator = (const RefCountObject& other) noexcept
    {
        return *this;
    }

    /**
     * @brief increase reference count by one
     */
    void ref() const noexcept
    {
        ms_count++;
    }

    /**
     * @brief Decrease reference count by one. If it is dropped to zero, the
     *        object is automatically destructed.
     * @return reference count value which is already decreased
     */
    int32 unref() const noexcept
    {
        int32 count = --ms_count;
        if (count == 0)
            delete this;
        return count;
    }

    /**
     * @brief get current reference count
     * @return count value
     */
    int32 get_ref_count() const noexcept
    {
        return ms_count.load();
    }

private:
    mutable AtomicObject<int32> ms_count;

    TREECORE_LEAK_DETECTOR(RefCountObject);
}; // class Object

inline void smart_ref(const RefCountObject* obj) noexcept
{ obj->ref(); }

inline int32 smart_unref(const RefCountObject* obj) noexcept
{ return obj->unref(); }

}

#endif // TREEFACE_OBJECT_H
