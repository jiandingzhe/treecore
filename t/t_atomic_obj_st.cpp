#include "treecore/AtomicObject.h"
#include "treecore/IntTypes.h"

#include "treecore/TestFramework.h"

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

        AtomicObject<FooEnum> var(FOO_FOO);
    }

    // 32 bit primitive type
    {
        AtomicObject<int32> var(111);
        IS(var.m_data, 111);
        IS(var.load(), 111);

        var.store(123);
        IS(var.m_data, 123);

        IS(var.exchange(456), 123);
        IS(var.m_data, 456);

        // arithmetic and logic ops
        // fetch-op
        IS(var++, 456);
        IS(var.m_data, 457);
        IS(var--, 457);
        IS(var.m_data, 456);

        // op-fetch
        IS(++var, 457);
        IS(--var, 456);

        IS(var += 123, 579);
        IS(var.m_data, 579);

        IS(var -= 321, 258);
        IS(var.m_data, 258);

        IS(var |= 14, 270);
        IS(var.m_data, 270);

        IS(var &= 28, 12);
        IS(var.m_data, 12);

        IS(var ^= 0xff, 243);
        IS(var.m_data, 243);

        // CAS
        OK(var.compare_set(243, 100));
        IS(var.m_data, 100);
        OK(!var.compare_set(123, 456));
        IS(var.m_data, 100);

        int target = 100;
        OK(var.compare_exchange(&target, 666));
        IS(var.m_data, 666);
        IS(target, 100);

        target = 123;
        OK(!var.compare_exchange(&target, 777));
        IS(var.m_data, 666);
        IS(target, 666);
    }

    // 64 bit primitive type
    {
        AtomicObject<uint64> var(0xf0f000ffff000f0f);
        IS(var.load(), 0xf0f000ffff000f0f);

        var.store(0xf0fff0f0ff0f0f0f);
        IS(var.m_data, 0xf0fff0f0ff0f0f0f);

        IS(var.exchange(0xfff000f0000ff0ff), 0xf0fff0f0ff0f0f0f);
        IS(var.m_data, 0xfff000f0000ff0ff);

        // arithmetic and logic ops
        // fetch-op
        IS(var++, 0xfff000f0000ff0ff);
        IS(var.m_data, 0xfff000f0000ff100);
        IS(var--, 0xfff000f0000ff100);
        IS(var.m_data, 0xfff000f0000ff0ff);

        // op-fetch
        IS(++var, 0xfff000f0000ff100);
        IS(--var, 0xfff000f0000ff0ff);
        IS(var |= 0xf0f0000000f00f00, 0xfff000f000ffffff);
        IS(var &= 0x0f0f0f0f0f0f0f0f, 0x0f000000000f0f0f);
        IS(var ^= 0x0fff0fff0fff0fff, 0x00ff0fff0ff000f0);

        // CAS
        OK(var.compare_set(0x00ff0fff0ff000f0, 0x0f1f3f7f0f1f3f7f));
        IS(var.m_data, 0x0f1f3f7f0f1f3f7f);
        OK(!var.compare_set(0xff00ff00ff00ff00, 0x0000000000000000));
        IS(var.m_data, 0x0f1f3f7f0f1f3f7f);

        uint64 target = 0x0f1f3f7f0f1f3f7f;
        OK(var.compare_exchange(&target, 0xff7f3f0fff7f3f0f));
        IS(var.m_data, 0xff7f3f0fff7f3f0f);
        IS(target, 0x0f1f3f7f0f1f3f7f);

        target = 123;
        OK(!var.compare_exchange(&target, 777));
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

        AtomicObject<Foo> var1({1, 2, 3});
        IS(var1.m_data.a, 1);
        IS(var1.m_data.b, 2);
        IS(var1.m_data.c, 3);

        {
            Foo tmp = var1.load();
            IS(tmp.a, 1);
            IS(tmp.b, 2);
            IS(tmp.c, 3);
        }

        var1.store({4, 5, 6});
        IS(var1.m_data.a, 4);
        IS(var1.m_data.b, 5);
        IS(var1.m_data.c, 6);

        {
            Foo tmp = var1.exchange({7, 8, 9});
            IS(var1.m_data.a, 7);
            IS(var1.m_data.b, 8);
            IS(var1.m_data.c, 9);
            IS(tmp.a, 4);
            IS(tmp.b, 5);
            IS(tmp.c, 6);
        }

        // CAS
        OK(var1.compare_set({7, 8, 9}, {10, 11, 12}));
        IS(var1.m_data.a, 10);
        IS(var1.m_data.b, 11);
        IS(var1.m_data.c, 12);
        OK(!var1.compare_set({100, 200, 300}, {111, 222, 333}));
        IS(var1.m_data.a, 10);
        IS(var1.m_data.b, 11);
        IS(var1.m_data.c, 12);

        {
            Foo tmp{10, 11, 12};
            OK(var1.compare_exchange(&tmp, {65, 43, 21}));
            IS(var1.m_data.a, 65);
            IS(var1.m_data.b, 43);
            IS(var1.m_data.c, 21);
            IS(tmp.a, 10);
            IS(tmp.b, 11);
            IS(tmp.c, 12);

            tmp = {99, 98, 97};
            OK(!var1.compare_exchange(&tmp, {2, 3, 4}));
            IS(var1.m_data.a, 65);
            IS(var1.m_data.b, 43);
            IS(var1.m_data.c, 21);
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

        AtomicObject<Foo> var1({1, 5, 12.34f});
        IS(var1.m_data.a, 1);
        IS(var1.m_data.b, 5);
        IS(var1.m_data.c, 12.34f);

        {
            Foo tmp = atomic_load(&var1);
            IS(tmp.a, 1);
            IS(tmp.b, 5);
            IS(tmp.c, 12.34f);
        }

        var1.store({5, 213, 34.89f});
        IS(var1.m_data.a, 5);
        IS(var1.m_data.b, 213);
        IS(var1.m_data.c, 34.89f);

        {
            Foo tmp = var1.exchange(Foo{890, 666, 666.666f});
            IS(tmp.a, 5);
            IS(tmp.b, 213);
            IS(tmp.c, 34.89f);
            IS(var1.m_data.a, 890);
            IS(var1.m_data.b, 666);
            IS(var1.m_data.c, 666.666f);
        }

        // CAS
        OK(var1.compare_set(Foo{890, 666, 666.666f}, Foo{1, 2, 34.5f}));
        IS(var1.m_data.a, 1);
        IS(var1.m_data.b, 2);
        IS(var1.m_data.c, 34.5f);
        OK(!var1.compare_set(Foo{1, 1, 1.0f}, Foo{1, 2, 3.0f}));
        IS(var1.m_data.a, 1);
        IS(var1.m_data.b, 2);
        IS(var1.m_data.c, 34.5f);

        {
            Foo expect{1, 2, 34.5f};
            OK(var1.compare_exchange(&expect, Foo{2, 3, 45.6f}));
            IS(expect.a, 1);
            IS(expect.b, 2);
            IS(expect.c, 34.5f);
            IS(var1.m_data.a, 2);
            IS(var1.m_data.b, 3);
            IS(var1.m_data.c, 45.6f);

            expect = {2, 2, 22.2f};
            OK(!var1.compare_exchange(&expect, Foo{1, 1, 11.11f}));
            IS(expect.a, 2);
            IS(expect.b, 3);
            IS(expect.c, 45.6f);
            IS(var1.m_data.a, 2);
            IS(var1.m_data.b, 3);
            IS(var1.m_data.c, 45.6f);
        }
    }
}
