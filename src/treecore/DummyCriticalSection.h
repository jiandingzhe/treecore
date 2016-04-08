#ifndef TREECORE_DUMMY_CRITICAL_SECTION_H
#define TREECORE_DUMMY_CRITICAL_SECTION_H

#include "treecore/PlatformDefs.h"

namespace treecore
{

/**
    A class that can be used in place of a real CriticalSection object, but which
    doesn't perform any locking.

    This is currently used by some templated classes, and most compilers should
    manage to optimise it out of existence.

    @see CriticalSection, Array, OwnedArray, ReferenceCountedArray
*/
class TREECORE_SHARED_API  DummyCriticalSection
{
public:
    inline DummyCriticalSection() noexcept      {}
    inline ~DummyCriticalSection() noexcept     {}

    inline void enter() const noexcept          {}
    inline bool tryEnter() const noexcept       { return true; }
    inline void exit() const noexcept           {}

    //==============================================================================
    /** A dummy scoped-lock type to use with a dummy critical section. */
    struct ScopedLockType
    {
        ScopedLockType (const DummyCriticalSection&) noexcept {}
    };

    /** A dummy scoped-unlocker type to use with a dummy critical section. */
    typedef ScopedLockType ScopedUnlockType;

private:
    TREECORE_DECLARE_NON_COPYABLE(DummyCriticalSection)
};

/**
 * @brief let you know if your critical section type is dummy
 */
template<typename T>
struct CriticalSectionIsDummy
{
    static const bool value = false;
};

template<>
struct CriticalSectionIsDummy<DummyCriticalSection>
{
    static const bool value = true;
};

} // namespace treecore

#endif // TREECORE_DUMMY_CRITICAL_SECTION_H
