#include "treecore/RefCountObject.h"
#include "treecore/RefCountSingleton.h"
#include "TestFramework.h"

struct Foo: public treecore::RefCountObject
{
    virtual ~Foo()
    {
        if (flag)
            *flag = false;
    }

    int foo = 0;
    float bar = 0.0f;
    bool* flag = nullptr;
};

void TestFramework::content()
{
    Foo* obj = treecore::RefCountSingleton<Foo>::getInstance();
    OK(obj);
    IS(obj->get_ref_count(), 1);

    bool flag = true;
    obj->flag = &flag;
    IS(treecore::RefCountSingleton<Foo>::releaseInstance(), 0);
    OK(!flag);

    flag = true;
    IS(treecore::RefCountSingleton<Foo>::releaseInstance(), -1);
    OK(flag);
}

