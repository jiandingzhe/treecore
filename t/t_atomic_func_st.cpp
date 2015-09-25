#include "treecore/AtomicFunc.h"
#include "TestFramework.h"

using namespace treecore;

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
}
