#include "TestFramework.h"

#include <treecore/AlignedMalloc.h>
#include <treecore/Array.h>
#include <treecore/IntTypes.h>
#include <treecore/SimdFunc.h>

using treecore::Array;
using treecore::SIMDType;
using treecore::pointer_sized_int;

TREECORE_ALN_BEGIN(32) struct Foo
{
    float a;
    treecore::int32 b;
    float c;
    TREECORE_ALIGNED_ALLOCATOR(Foo)
} TREECORE_ALN_END(32);


void TestFramework::content()
{
    IS(TREECORE_ALIGNOF(Foo), 32);
    IS(sizeof(Foo), 32);

    {
        Array<Foo, 32> array1;
        for (int i = 0; i < 100; i++)
        {
            array1.add(Foo());
        }

        pointer_sized_int init_addr_1 = pointer_sized_int(&array1[0]);
        IS(init_addr_1 % 32, 0);
    }

    {
        Array<Foo, 32> array2;
        for (int i = 0; i < 100; i++)
        {
            Foo tmp;
            array2.add(tmp);
        }

        pointer_sized_int init_addr_2 = pointer_sized_int(&array2[0]);
        IS(init_addr_2 % 32, 0);
    }
}
