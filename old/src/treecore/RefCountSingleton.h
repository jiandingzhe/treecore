#ifndef TREECORE_REF_COUNT_SINGLETON_H
#define TREECORE_REF_COUNT_SINGLETON_H

#include "treecore/AtomicObject.h"
#include "treecore/IntTypes.h"
#include "treecore/RefCountHolder.h"
#include "treecore/Thread.h"

class TestFramework;

namespace treecore
{

template<typename T>
class RefCountSingleton
{
    friend class ::TestFramework;

public:
    /**
     * @brief get object global instance or build it
     * @return a pointer object
     */
    static T* getInstance()
    {
        T* tmp = nullptr;

        // early out
        if (!get_building_lock())
        {
            tmp = get_raw_instance().load();
            if (tmp)
                return tmp;
        }

        // do build
        while(get_building_lock().compare_set(0, 1))
            Thread::yield();

        tmp = get_raw_instance().load();
        if (!tmp)
        {
            tmp = new T();
            smart_ref(tmp);
            get_raw_instance() = tmp;
        }

        while (get_building_lock().compare_set(1, 0))
            Thread::yield();

        return tmp;
    }

    /**
     * @brief release global hold of the instance
     * @return remaining reference count after unhold
     */
    static int32 releaseInstance()
    {
        // early out if it is already empty
        T* tmp = get_raw_instance().load();
        if (!tmp)
            return -1;

        // do release

        // FIXME thread ABA safety?
        // thread1     thread2
        //             get ptr
        // --refcnt
        // delete obj
        //             ++refcnt
        while (get_building_lock().compare_set(0, 1))
            Thread::yield();

        tmp = get_raw_instance().load();
        int cnt_before_release = -1;
        if (tmp)
        {
            cnt_before_release = smart_unref(tmp);
            get_raw_instance() = nullptr;
        }

        while (get_building_lock().compare_set(1, 0))
            Thread::yield();

        return cnt_before_release;
    }

private:

    static AtomicObject<int>& get_building_lock()
    {
        static AtomicObject<int> m_building_fuck_cxx;
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
