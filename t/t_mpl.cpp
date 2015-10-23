#include "treecore/MPL.h"

#include "TestFramework.h"

struct Foo
{
};

struct Bar
{
};

template<typename T> struct Baz
{
};

void TestFramework::content()
{
    bool value_int_int = treecore::mpl_same_type<int, int>::value;
    OK(! value_int_int);

    bool value_int_float = treecore::mpl_same_type<int, float>::value;
    OK(! value_int_float);

    bool value_foo_foo = treecore::mpl_same_type<Foo, Foo>::value;
    OK(value_foo_foo);

    bool value_foo_bar = treecore::mpl_same_type<Foo, Bar>::value;
    OK(! value_foo_bar);

    bool value_baz_int_int = treecore::mpl_same_type<Baz<int>, Baz<int>>::value;
    OK(value_baz_int_int);

    bool value_baz_int_float = treecore::mpl_same_type<Baz<int>, Baz<float>>::value;
    OK(!value_baz_int_float);
}
