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
    static T* getInstance()
    {
        // early out
        T* tmp = m_instance.get();
        if (tmp)
            return tmp;

        // do build
        while(m_building.compare_set(0, 1))
            Thread::yield();

        tmp = m_instance.get();
        if (!tmp)
        {
            tmp = new T();
            smart_ref(tmp); // unqualified call to smart_ref
            m_instance = tmp;
        }

        while (m_building.compare_set(1, 0))
            Thread::yield();

        return tmp;
    }

    static int32 releaseInstance()
    {
        // early out if it is already empty
        T* tmp = m_instance.get();
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

        m_instance = nullptr;
        int32 cnt = smart_unref(tmp);

        while (m_building.compare_set(1, 0))
            Thread::yield();

        return cnt;
    }

private:
    static AtomicObject<int> m_building;
    static RefCountHolder<T> m_instance;
};

template<typename T>
TREECORE_SELECT_ANY AtomicObject<int> RefCountSingleton<T>::m_building(0);

template<typename T>
TREECORE_SELECT_ANY RefCountHolder<T> RefCountSingleton<T>::m_instance(nullptr);


} // namespace treecore

#endif // TREECORE_REF_COUNT_SINGLETON_H
