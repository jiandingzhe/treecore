#include "TestFramework.h"

#include "treecore/Array.h"
#include "treecore/ObjectPool.h"


struct Foo
{
    Foo(bool* flag): a(0), b(0.0f), flag(flag)
    {
        *flag = true;
    }

    Foo(int a, float b, bool* flag): a(a), b(b), flag(flag)
    {
        *flag = true;
    }

    ~Foo()
    {
        *flag = false;
    }

    treecore::int32 a;
    float b;
    bool* flag;
};

void TestFramework::content()
{
    // tests for non-threaded queue
    {
        bool flags[20] = {};
        treecore::ObjectPool<Foo, false, 128> pool;
        treecore::Array<Foo*> objs;

        for (int i = 0; i < 10; i++)
        {
            Foo* obj = pool.generate(&flags[i]);
            IS(obj->a, 0);
            IS(obj->b, 0.0);
            IS(obj->flag, &flags[i]);
            OK(flags[i]);
            objs.add(obj);
        }

        for (int i = 0; i < 10; i++)
        {
            Foo* obj = pool.generate(i, 12.3f, &flags[i+10]);
            IS(obj->a, i);
            IS(obj->b, 12.3f);
            IS(obj->flag, &flags[i+10]);
            OK(flags[i+10]);
            objs.add(obj);
        }

        for (int i = 0; i < objs.size(); i++)
        {
            pool.recycle(objs[i]);
            OK(!flags[i]);
        }

    }

    // tests for lock-free queue
    {
        bool flags[20] = {};
        treecore::ObjectPool<Foo, true, 128> pool;
        treecore::Array<Foo*> objs;

        for (int i = 0; i < 10; i++)
        {
            Foo* obj = pool.generate(&flags[i]);
            IS(obj->a, 0);
            IS(obj->b, 0.0);
            IS(obj->flag, &flags[i]);
            OK(flags[i]);
            objs.add(obj);
        }

        for (int i = 0; i < 10; i++)
        {
            Foo* obj = pool.generate(i, 12.3f, &flags[i+10]);
            IS(obj->a, i);
            IS(obj->b, 12.3f);
            IS(obj->flag, &flags[i+10]);
            OK(flags[i+10]);
            objs.add(obj);
        }

        for (int i = 0; i < objs.size(); i++)
        {
            pool.recycle(objs[i]);
            OK(!flags[i]);
        }

    }
}
