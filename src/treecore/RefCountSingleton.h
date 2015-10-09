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
        if (!m_building)
        {
            tmp = m_instance.load();
            if (tmp)
                return tmp;
        }

        // do build
        while(m_building.compare_set(0, 1))
            Thread::yield();

        tmp = m_instance.load();
        if (!tmp)
        {
            tmp = new T();
            smart_ref(tmp);
            m_instance = tmp;
        }

        while (m_building.compare_set(1, 0))
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
        T* tmp = m_instance.load();
        if (!tmp)
            return -1;

        // do release

        // FIXME thread ABA safety?
        // thread1     thread2
        //             get ptr
        // --refcnt
        // delete obj
        //             ++refcnt
        while (m_building.compare_set(0, 1))
            Thread::yield();

        tmp = m_instance.load();
        int cnt_before_release = -1;
        if (tmp)
        {
            cnt_before_release = smart_unref(tmp);
            m_instance = nullptr;
        }

        while (m_building.compare_set(1, 0))
            Thread::yield();

        return cnt_before_release;
    }

private:
    static AtomicObject<int> m_building;
    static AtomicObject<T*> m_instance;
};

template<typename T>
TREECORE_SELECT_ANY AtomicObject<int> RefCountSingleton<T>::m_building;

template<typename T>
TREECORE_SELECT_ANY AtomicObject<T*> RefCountSingleton<T>::m_instance(nullptr);


} // namespace treecore

#endif // TREECORE_REF_COUNT_SINGLETON_H
