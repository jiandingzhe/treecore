#include "TestFramework.h"

#include "treecore/AlignedMalloc.h"
#include "treecore/RefCountHolder.h"
#include "treecore/IntTypes.h"
#include "treecore/SIMD.h"

using namespace treecore;

struct Foo: public AlignedMalloc<32>
{
    int a = 0;
    float b = 0;
};

struct Bar
{
    Foo c;
};

struct Vec4f
{
    Vec4f(float x, float y, float z, float w)
    {
        data = simd_set<float, 16>(x, y, z, w);
    }

    float get_x() const
    {
        return simd_get_one<0, float, 16>(data);
    }

    float get_y() const
    {
        return simd_get_one<1, float, 16>(data);
    }

    float get_z() const
    {
        return simd_get_one<2, float, 16>(data);
    }

    float get_w() const
    {
        return simd_get_one<3, float, 16>(data);
    }

    SIMDType<16> data;
};

void TestFramework::content()
{
    Foo* foo = new Foo();
    OK(foo);
    IS(pointer_sized_int(foo) % 32, 0);

    Vec4f* baz = (Vec4f*) treecore::aligned_malloc(sizeof(Vec4f), sizeof(float)*4);
    OK(baz);
    IS(pointer_sized_int(baz) % 16, 0);

    new (baz) Vec4f(1,2,3,4);
    IS(baz->get_x(), 1);
    IS(baz->get_y(), 2);
    IS(baz->get_z(), 3);
    IS(baz->get_w(), 4);

    void* aaa = treecore::aligned_malloc<0>(4);
    OK(aaa);
}
