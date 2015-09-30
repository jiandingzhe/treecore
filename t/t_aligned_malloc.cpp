#include "TestFramework.h"

#include "treecore/AlignedMalloc.h"
#include "treecore/RefCountHolder.h"
#include "treecore/IntTypes.h"
#include "treecore/SIMD.h"

using namespace treecore;

TREECORE_ALN_BEGIN(32)
struct Aaa
{
    float a;
    float b;
    int32 c;

    TREECORE_ALIGNED_ALLOCATOR(Aaa)
} TREECORE_ALN_END(32);

TREECORE_ALN_BEGIN(16)
struct Foo
{
    int a = 0;
    float b = 0;

    TREECORE_ALIGNED_ALLOCATOR(Foo)
} TREECORE_ALN_END(16);

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
    {
        void* aaa = treecore::aligned_malloc<0>(4);
        OK(aaa);
        treecore::aligned_free<0>(aaa);
    }

    {
        Aaa* a = new Aaa();
        Aaa* b = new Aaa();
        Aaa* c = new Aaa();
        Aaa* d = new Aaa();
        Aaa* e = new Aaa();
        Aaa* f = new Aaa();
        IS(pointer_sized_int(a) % 32, 0);
        IS(pointer_sized_int(b) % 32, 0);
        IS(pointer_sized_int(c) % 32, 0);
        IS(pointer_sized_int(d) % 32, 0);
        IS(pointer_sized_int(e) % 32, 0);
        IS(pointer_sized_int(f) % 32, 0);
        delete a;
        delete b;
        delete c;
        delete d;
        delete e;
        delete f;
    }

    {
        Foo* foo1 = new Foo();
        Foo* foo2 = new Foo();
        Foo* foo3 = new Foo();
        Foo* foo4 = new Foo();
        OK(foo1);
        OK(foo2);
        OK(foo3);
        OK(foo4);
        IS(pointer_sized_int(foo1) % 16, 0);
        IS(pointer_sized_int(foo2) % 16, 0);
        IS(pointer_sized_int(foo3) % 16, 0);
        IS(pointer_sized_int(foo4) % 16, 0);
        delete foo1;
        delete foo2;
        delete foo3;
        delete foo4;
    }

    {
        Vec4f* baz = (Vec4f*) treecore::aligned_malloc(sizeof(Vec4f), sizeof(float)*4);
        OK(baz);
        IS(pointer_sized_int(baz) % 16, 0);

        new (baz) Vec4f(1,2,3,4);
        IS(baz->get_x(), 1);
        IS(baz->get_y(), 2);
        IS(baz->get_z(), 3);
        IS(baz->get_w(), 4);
    }
}
