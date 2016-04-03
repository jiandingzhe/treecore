#include "treecore/AtomicFunc.h"
#include "treecore/IntTypes.h"

#include "TestFramework.h"

using namespace treecore;

void TestFramework::content()
{
    // enum type
    {
        enum FooEnum
        {
            FOO_FOO,
            FOO_BAR
        };

        FooEnum var = FOO_FOO;
        atomic_store(&var, FOO_BAR);
    }
    // 32 bit primitive type
    {
        int32 var = 111;
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

    // 64 bit primitive type
    {
        uint64 var = 0xf0f000ffff000f0f;
        IS(atomic_load(&var), 0xf0f000ffff000f0f);

        atomic_store(&var, 0xf0fff0f0ff0f0f0f);
        IS(var, 0xf0fff0f0ff0f0f0f);

        IS(atomic_exchange(&var, 0xfff000f0000ff0ff), 0xf0fff0f0ff0f0f0f);
        IS(var, 0xfff000f0000ff0ff);

        // arithmetic and logic ops
        // fetch-op
        IS(atomic_fetch_add<uint64>(&var, 0x000ff00ff0000f00), 0xfff000f0000ff0ff);
        IS(var, 0xfffff0fff00fffff);
        IS(atomic_fetch_sub<uint64>(&var, 0x000ff00ff0000f00), 0xfffff0fff00fffff);
        IS(var, 0xfff000f0000ff0ff);
        IS(atomic_fetch_or<uint64>(&var, 0x00ff00ff00ff00ff), 0xfff000f0000ff0ff);
        IS(var, 0xffff00ff00fff0ff);
        IS(atomic_fetch_and<uint64>(&var, 0xf0f0f0f0f0f0f0f0), 0xffff00ff00fff0ff);
        IS(var, 0xf0f000f000f0f0f0);
        IS(atomic_fetch_xor<uint64>(&var, 0xffff0000ffff0000), 0xf0f000f000f0f0f0);
        IS(var, 0x0f0f00f0ff0ff0f0);

        // op-fetch
        IS(atomic_add_fetch<uint64>(&var, 0xf0f0000000f00f00), 0xffff00f0fffffff0);
        IS(atomic_sub_fetch<uint64>(&var, 0x0f0f00000f0f0000), 0xf0f000f0f0f0fff0);
        IS(atomic_or_fetch<uint64>(&var, 0xff00ff00ff00ff00), 0xfff0fff0fff0fff0);
        IS(atomic_and_fetch<uint64>(&var, 0x0f0f0f0f0f0f0f0f), 0x0f000f000f000f00);
        IS(atomic_xor_fetch<uint64>(&var, 0x0fff0fff0fff0fff), 0x00ff00ff00ff00ff);

        // CAS
        OK(atomic_compare_set<uint64>(&var, 0x00ff00ff00ff00ff, 0x0f1f3f7f0f1f3f7f));
        IS(var, 0x0f1f3f7f0f1f3f7f);
        OK(!atomic_compare_set<uint64>(&var, 0xff00ff00ff00ff00, 0x0000000000000000));
        IS(var, 0x0f1f3f7f0f1f3f7f);

        uint64 target = 0x0f1f3f7f0f1f3f7f;
        OK(atomic_compare_exchange<uint64>(&var, &target, 0xff7f3f0fff7f3f0f));
        IS(var, 0xff7f3f0fff7f3f0f);
        IS(target, 0x0f1f3f7f0f1f3f7f);

        target = 123;
        OK(!atomic_compare_exchange<uint64>(&var, &target, 777));
        IS(var, 0xff7f3f0fff7f3f0f);
        IS(target, 0xff7f3f0fff7f3f0f);
    }

    // 32 bit composite type
    {
        struct Foo
        {
            int8 a;
            uint8 b;
            int16 c;
        };
        IS(sizeof(Foo), 4);

        Foo var1{1, 2, 3};
        {
            Foo tmp = atomic_load(&var1);
            IS(tmp.a, 1);
            IS(tmp.b, 2);
            IS(tmp.c, 3);
        }

        atomic_store<Foo>(&var1, {4, 5, 6});
        IS(var1.a, 4);
        IS(var1.b, 5);
        IS(var1.c, 6);

        {
            Foo tmp = atomic_exchange<Foo>(&var1, {7, 8, 9});
            IS(var1.a, 7);
            IS(var1.b, 8);
            IS(var1.c, 9);
            IS(tmp.a, 4);
            IS(tmp.b, 5);
            IS(tmp.c, 6);
        }

        // CAS
        OK(atomic_compare_set<Foo>(&var1, {7, 8, 9}, {10, 11, 12}));
        IS(var1.a, 10);
        IS(var1.b, 11);
        IS(var1.c, 12);
        OK(!atomic_compare_set<Foo>(&var1, {100, 200, 300}, {111, 222, 333}));
        IS(var1.a, 10);
        IS(var1.b, 11);
        IS(var1.c, 12);

        {
            Foo tmp{10, 11, 12};
            OK(atomic_compare_exchange<Foo>(&var1, &tmp, {65, 43, 21}));
            IS(var1.a, 65);
            IS(var1.b, 43);
            IS(var1.c, 21);
            IS(tmp.a, 10);
            IS(tmp.b, 11);
            IS(tmp.c, 12);

            tmp = {99, 98, 97};
            OK(!atomic_compare_exchange<Foo>(&var1, &tmp, {2, 3, 4}));
            IS(var1.a, 65);
            IS(var1.b, 43);
            IS(var1.c, 21);
            IS(tmp.a, 65);
            IS(tmp.b, 43);
            IS(tmp.c, 21);
        }
    }

    // 64 bit composite type
    {
        struct Foo
        {
            int16  a;
            uint16 b;
            float  c;
        };
        IS(sizeof(Foo), 8);

        Foo var1{1, 5, 12.34f};
        {
            Foo tmp = atomic_load(&var1);
            IS(tmp.a, 1);
            IS(tmp.b, 5);
            IS(tmp.c, 12.34f);
        }

        atomic_store<Foo>(&var1, {5, 213, 34.89f});
        IS(var1.a, 5);
        IS(var1.b, 213);
        IS(var1.c, 34.89f);

        {
            Foo tmp = atomic_exchange(&var1, Foo{890, 666, 666.666f});
            IS(tmp.a, 5);
            IS(tmp.b, 213);
            IS(tmp.c, 34.89f);
            IS(var1.a, 890);
            IS(var1.b, 666);
            IS(var1.c, 666.666f);
        }

        // CAS
        OK(atomic_compare_set(&var1, Foo{890, 666, 666.666f}, Foo{1, 2, 34.5f}));
        IS(var1.a, 1);
        IS(var1.b, 2);
        IS(var1.c, 34.5f);
        OK(!atomic_compare_set(&var1, Foo{1, 1, 1.0f}, Foo{1, 2, 3.0f}));
        IS(var1.a, 1);
        IS(var1.b, 2);
        IS(var1.c, 34.5f);

        {
            Foo expect{1, 2, 34.5f};
            OK(atomic_compare_exchange(&var1, &expect, Foo{2, 3, 45.6f}));
            IS(expect.a, 1);
            IS(expect.b, 2);
            IS(expect.c, 34.5f);
            IS(var1.a, 2);
            IS(var1.b, 3);
            IS(var1.c, 45.6f);

            expect = {2, 2, 22.2f};
            OK(!atomic_compare_exchange(&var1, &expect, Foo{1, 1, 11.11f}));
            IS(expect.a, 2);
            IS(expect.b, 3);
            IS(expect.c, 45.6f);
            IS(var1.a, 2);
            IS(var1.b, 3);
            IS(var1.c, 45.6f);
        }
    }
}
