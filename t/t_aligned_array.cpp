#include "TestFramework.h"

#include <treecore/AlignedMalloc.h>
#include <treecore/Array.h>
#include <treecore/IntTypes.h>
#include <treecore/SIMD.h>

using treecore::Array;
using treecore::AlignedMalloc;
using treecore::SIMDType;
using treecore::pointer_sized_int;

struct Foo: AlignedMalloc<32>
{
    SIMDType<16> a;
    SIMDType<16> b;
};

void TestFramework::content()
{
    {
        Array<Foo, 32> array1;
        for (int i = 0; i < 100; i++)
        {
            array1.add(Foo());
        }

        pointer_sized_int init_addr_1 = pointer_sized_int(&array1.getReference(0));
        IS(init_addr_1 % 32, 0);
    }

    {
        Array<Foo, 32> array2;
        for (int i = 0; i < 100; i++)
        {
            Foo tmp;
            array2.add(tmp);
        }

        pointer_sized_int init_addr_2 = pointer_sized_int(&array2.getReference(0));
        IS(init_addr_2 % 32, 0);
    }
}