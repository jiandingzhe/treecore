#ifndef TREECORE_REF_COUNT_SINGLETON_H
#define TREECORE_REF_COUNT_SINGLETON_H

#include "treecore/AtomicObject.h"
#include "treecore/Thread.h"
#include "treecore/IntTypes.h"

class TestFramework;

namespace treecore
{

template<typename T>
class RefCountSingleton
{
    friend class ::TestFramework;

    static T* getInstance()
    {
        // early out
        T* tmp = atomic_load(&m_instance);
        if (tmp)
            return tmp;

        // do build
        while(m_building.compare_set(0, 1))
            Thread::yield();

        if (!m_instance)
        {
            tmp = new T();
            smart_ref(tmp); // unqualified call to smart_ref
            atomic_store(&m_instance, tmp);
        }

        while (m_building.compare_set(1, 0))
            Thread::yield();

        return m_instance;
    }

    static int32 releaseInstance()
    {
        // early out if it is already empty
        T* tmp = atomic_load(&m_instance);
        if (!tmp)
            return -1;

        // do release
        // FIXME is it possible that:
        // thread1     thread2
        //             get ptr
        // --refcnt
        // delete obj
        //             ++refcnt
        while (m_building.compare_set(0, 1))
            Thread::yield();

        atomic_store(&m_instance, nullptr);
        int32 cnt = smart_unref(tmp);

        while (m_building.compare_set(1, 0))
            Thread::yield();

        return cnt;
    }

private:
    static AtomicObject<int> m_building;
    static T* m_instance;
};

#define TREECORE_IMPLEMENT_REF_COUNT_SINGLETON(_type_) \
    template<>\
    treecore::AtomicObject<int> treecore::RefCountSingleton<_type_>::m_building(0);\
    template<>\
    _type_* treecore::RefCountSingleton<_type_>::m_instance = nullptr;

} // namespace treecore

#endif // TREECORE_REF_COUNT_SINGLETON_H
