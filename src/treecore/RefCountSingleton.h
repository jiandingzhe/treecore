#ifndef TREECORE_REF_COUNT_SINGLETON_H
#define TREECORE_REF_COUNT_SINGLETON_H

#include "treecore/ReadWriteLock.h"
#include "treecore/IntTypes.h"
#include "treecore/RefCountHolder.h"
#include "treecore/Thread.h"
#include "treecore/ScopedWriteLock.h"

class TestFramework;

namespace treecore
{

///
/// \brief manage a global instance by managing reference count
///
/// Type must have smart_ref(Type*) function that increase reference count, and
/// have smart_unref(Type*) function that decrease ref & destroy when dropped to
/// zero. The latter function must also return an integer which is the reference
/// count after release occurred.
///
template<typename T>
class RefCountSingleton
{
    friend class ::TestFramework;

public:
    ///
    /// \brief get object global instance or build it
    /// \return a pointer object
    ///
    static RefCountHolder<T> getInstance()
    {
        RefCountHolder<T> re;
        ReadWriteLock& mutex = get_building_mutex();

        // early out
        if ( mutex.tryEnterRead() )
        {
            re = get_raw_instance().load();
            if (re)
            {
                mutex.exitRead();
                return re;
            }

            mutex.exitRead();
        }

        // do build
        {
            ScopedWriteLock lock( mutex );

            T* tmp = get_raw_instance().load();
            if (tmp == nullptr)
            {
                tmp = new T();
                smart_ref( tmp );
                get_raw_instance() = tmp;
            }

            re = tmp;
            return re;
        }
    }

    ///
    /// \brief get global instance instantly, even it is not initialized
    /// \return a pointer object, may contain empty value
    ///
    static RefCountHolder<T> getInstanceWithoutCreating()
    {
        RefCountHolder<T> re;
        ReadWriteLock& lock = get_building_mutex();

        if ( lock.tryEnterRead() )
        {
            re = get_raw_instance().load();
            lock.exitRead();
        }

        return re;
    }

    /**
     * @brief release global hold of the instance
     * @return remaining reference count after unhold, -1 if it is empty
     */
    static int32 releaseInstance()
    {
        // early out if it is already empty
        T* tmp = get_raw_instance().load();
        if (tmp == nullptr)
            return -1;

        // do release
        {
            ScopedWriteLock lock( get_building_mutex() );

            tmp = get_raw_instance().load();
            int cnt_before_release = -1;

            // do actual release
            if (tmp != nullptr)
            {
                cnt_before_release = smart_unref( tmp );
                get_raw_instance() = nullptr;
            }
            return cnt_before_release;
        }
    }

private:

    static ReadWriteLock& get_building_mutex()
    {
        static ReadWriteLock m_building_fuck_cxx;
        return m_building_fuck_cxx;
    }

    static AtomicObject<T*>& get_raw_instance()
    {
        static AtomicObject<T*> m_instance_fuck_cxx;
        return m_instance_fuck_cxx;
    }
};

} // namespace treecore

#endif // TREECORE_REF_COUNT_SINGLETON_H
