#include "treecore/DList.h"
#include "TestFramework.h"

struct Foo: public treecore::DListNode<Foo>
{
    Foo(int arg): content(arg) {}
    virtual ~Foo() {}
    int content = 0;
};

typedef treecore::DList<Foo> ListType;

void TestFramework::content()
{
    ListType list;

    Foo foo1(1);
    Foo foo2(2);

    list.push_tail(&foo1);
    list.push_tail(&foo2);

}
