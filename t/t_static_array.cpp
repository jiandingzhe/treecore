#include "treecore/StaticArray.h"
#include "treecore/IntTypes.h"

#include "TestFramework.h"

using treecore::pointer_sized_uint;

TREECORE_ALN_BEGIN(16) struct Foo
{
    char a;
    char b;
    char c;
    char d;
} TREECORE_ALN_END(16);

TREECORE_ALN_BEGIN(32) struct Bar
{
    char a;
} TREECORE_ALN_END(32);


void TestFramework::content()
{
    char fuck = 0;
    IS(fuck, 0);

    treecore::StaticArray<Foo, 12> foo_array;
    IS(foo_array.size, 12);
    IS(pointer_sized_uint(&foo_array[0]) % 16, 0);
    IS(pointer_sized_uint(&foo_array[1]) % 16, 0);
    IS(pointer_sized_uint(&foo_array[10]) % 16, 0);
    IS(pointer_sized_uint(&foo_array[11]) % 16, 0);

    char fuck_again = 1;
    IS(fuck_again, 1);

    treecore::StaticArray<int, 11> int_array;
    IS(int_array.size, 11);
    IS(pointer_sized_uint(&int_array[0]) % TREECORE_ALIGNOF(int), 0);
    IS(pointer_sized_uint(&int_array[1]) % TREECORE_ALIGNOF(int), 0);
    IS(pointer_sized_uint(&int_array[9]) % TREECORE_ALIGNOF(int), 0);
    IS(pointer_sized_uint(&int_array[10]) % TREECORE_ALIGNOF(int), 0);

    char fuck_yet_again = 3;
    IS(fuck_yet_again, 3);
    treecore::StaticArray<Bar, 13> bar_array;
    IS(bar_array.size, 13);
    IS(pointer_sized_uint(&bar_array[0]) % 32, 0);
    IS(pointer_sized_uint(&bar_array[1]) % 32, 0);
    IS(pointer_sized_uint(&bar_array[11]) % 32, 0);
    IS(pointer_sized_uint(&bar_array[12]) % 32, 0);
}
