#include "treecore/AtomicFunc.h"
#include "treecore/IntTypes.h"

#include "TestFramework.h"

using namespace treecore;

struct Foo
{
    int16  a;
    uint16 b;
    float  c;
};

void TestFramework::content()
{
    {
        int var = 111;
        IS(atomic_load(&var), 111);

        atomic_store(&var, 123);
        IS(var, 123);

        IS(atomic_exchange(&var, 456), 123);
        IS(var, 456);

        // arithmetic and logic ops
        // fetch-op
        IS(atomic_fetch_add(&var, 100), 456);
        IS(var, 556);
        IS(atomic_fetch_sub(&var, 200), 556);
        IS(var, 356);
        IS(atomic_fetch_or(&var, 0xf0), 356);
        IS(var, 500);
        IS(atomic_fetch_and(&var, 56), 500);
        IS(var, 48);
        IS(atomic_fetch_xor(&var, 0xfc), 48);
        IS(var, 204);

        // op-fetch
        IS(atomic_add_fetch(&var, 123), 327);
        IS(atomic_sub_fetch(&var, 321), 6);
        IS(atomic_or_fetch(&var, 20), 22);
        IS(atomic_and_fetch(&var, 28), 20);
        IS(atomic_xor_fetch(&var, 0xff), 235);

        // CAS
        OK(atomic_compare_set(&var, 235, 100));
        IS(var, 100);
        OK(!atomic_compare_set(&var, 123, 456));
        IS(var, 100);

        int target = 100;
        OK(atomic_compare_exchange(&var, &target, 666));
        IS(var, 666);
        IS(target, 100);

        target = 123;
        OK(!atomic_compare_exchange(&var, &target, 777));
        IS(var, 666);
        IS(target, 666);
    }

    {
        Foo var1{1, 5, 12.34f};
        {
            Foo tmp = atomic_load(&var1);
            IS(tmp.a, 1);
            IS(tmp.b, 5);
            IS(tmp.c, 12.34f);
        }

        atomic_store(&var1, {5, 213, 34.89f});
        IS(var1.a, 5);
        IS(var1.b, 213);
        IS(var1.c, 34.89f);

        {
            Foo tmp = atomic_exchange(&var1, {890, 666, 666.666f});
            IS(tmp.a, 5);
            IS(tmp.b, 213);
            IS(tmp.c, 34.89f);
            IS(var1.a, 890);
            IS(var1.b, 666);
            IS(var1.c, 666.666f);
        }

        // CAS
        OK(atomic_compare_set(&var1, {890, 666, 666.666f}, {1, 2, 34.5f}));
        IS(var1.a, 1);
        IS(var1.b, 2);
        IS(var1.c, 34.5f);
        OK(!atomic_compare_set(&var1, {1, 1, 1.0f}, {1, 2, 3.0f}));
        IS(var1.a, 1);
        IS(var1.b, 2);
        IS(var1.c, 34.5f);

        {
            Foo expect{1, 2, 34.5f};
            OK(atomic_compare_exchange(&var1, &expect, {2, 3, 45.6f}));
            IS(expect.a, 1);
            IS(expect.b, 2);
            IS(expect.c, 34.5f);
            IS(var1.a, 2);
            IS(var1.b, 3);
            IS(var1.c, 45.6f);

            expect = {2, 2, 22.2f};
            OK(!atomic_compare_exchange(&var1, &expect, {1, 1, 11.11f}));
            IS(expect.a, 2);
            IS(expect.b, 3);
            IS(expect.c, 45.6f);
            IS(var1.a, 2);
            IS(var1.b, 3);
            IS(var1.c, 45.6f);
        }
    }
}
