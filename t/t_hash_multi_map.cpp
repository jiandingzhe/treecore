#include "TestFramework.h"

#include "treecore/HashMultiMap.h"

using namespace treecore;

void TestFramework::content()
{
    typedef HashMultiMap<int, int> MapType;
    MapType map;
    OK("map created");
    IS(map.size(), 0);

    map.store(1, 10);
    OK("map insert value");
    IS(map.size(), 1);
    IS(map.numKeys(), 1);
    OK(map.contains(1));
    OK(map.contains(1, 10));
    OK(map.containsValue(10));
    IS(map.count(1), 1);

    map.store(1, 2);
    OK("map insert value");
    IS(map.size(), 2);
    IS(map.numKeys(), 1);
    OK(map.contains(1));
    OK(map.contains(1, 10));
    OK(map.contains(1, 2));
    OK(map.containsValue(10));
    OK(map.containsValue(2));
    IS(map.count(1), 2);

    map.store(4, 3);
    OK("map insert value using another key");
    IS(map.size(), 3);
    IS(map.numKeys(), 2);
    OK(map.contains(1));
    OK(map.contains(4));
    OK(map.contains(1, 10));
    OK(map.contains(1, 2));
    OK(map.contains(4, 3));
    OK(map.containsValue(10));
    OK(map.containsValue(2));
    OK(map.containsValue(3));
    IS(map.count(1), 2);
    IS(map.count(4), 1);

    map.store(700, 1);
    OK("map insert values using yet another key");
    IS(map.size(), 4);
    IS(map.numKeys(), 3);
    OK(map.contains(1));
    OK(map.contains(4));
    OK(map.contains(700));
    OK(map.contains(1, 10));
    OK(map.contains(1, 2));
    OK(map.contains(4, 3));
    OK(map.contains(700, 1));
    OK(map.containsValue(10));
    OK(map.containsValue(2));
    OK(map.containsValue(3));
    OK(map.containsValue(1));
    IS(map.count(1), 2);
    IS(map.count(4), 1);
    IS(map.count(700), 1);

    {
        MapType::Iterator i(map);
        OK(map.select(1, i));
        OK(i.hasContent());
        IS(i.key(), 1);
        IS(i.values().size(), 2);
        OK(i.values().contains(2));
        OK(i.values().contains(10));

        bool got_2 = false;
        bool got_10 = false;

        if (i.value() == 2)
            got_2 = true;
        else if (i.value() == 10)
            got_10 = true;
        else
            abort();

        OK(i.next());
        if (i.value() == 2)
            got_2 = true;
        else if (i.value() == 10)
            got_10 = true;
        else
            abort();

        OK(got_2);
        OK(got_10);
    }

    //
    // iterate
    //
    {
        bool got_1_10 = false;
        bool got_1_2 = false;
        bool got_4_3 = false;
        bool got_700_1 = false;
        int n_passed = 0;

        MapType::Iterator it(map);
        OK("iterate");

        while (it.next())
        {
            n_passed++;
            if (it.key() == 1)
            {
                OK(it.hasContent());
                IS(it.values().size(), 2);

                switch (it.value())
                {
                case 2:
                    got_1_2 = true; break;
                case 10:
                    got_1_10 = true; break;
                default:
                    abort();
                }
            }
            else if (it.key() == 4)
            {
                OK(it.hasContent());
                IS(it.values().size(), 1);

                if (it.value() == 3)
                    got_4_3 = true;
                else
                    abort();
            }
            else if (it.key() == 700)
            {
                OK(it.hasContent());
                IS(it.values().size(), 1);

                if (it.value() == 700)
                    got_700_1 = true;
                else
                    abort();
            }
        }

        OK(got_1_10);
        OK(got_1_2);
        OK(got_4_3);
        OK(got_700_1);
        IS(n_passed, 4);
    }

    {
        bool got_1_10 = false;
        bool got_1_2 = false;
        bool got_4_3 = false;
        bool got_700_1 = false;
        int n_passed = 0;

        MapType::Iterator it(map);
        OK("iterate");

        while (it.nextKey())
        {
            n_passed++;
            if (it.key() == 1)
            {
                OK(it.hasContent());
                IS(it.values().size(), 2);

                for (int i = 0; i < it.values().size(); i++)
                {
                    switch (it.values()[i])
                    {
                    case 2:
                        got_1_2 = true; break;
                    case 10:
                        got_1_10 = true; break;
                    default:
                        abort();
                    }
                }
            }
            else if (it.key() == 4)
            {
                OK(it.hasContent());
                IS(it.values().size(), 1);

                if (it.value() == 3)
                    got_4_3 = true;
                else
                    abort();
            }
            else if (it.key() == 700)
            {
                OK(it.hasContent());
                IS(it.values().size(), 1);

                if (it.value() == 700)
                    got_700_1 = true;
                else
                    abort();
            }
        }

        OK(got_1_10);
        OK(got_1_2);
        OK(got_4_3);
        OK(got_700_1);
        IS(n_passed, 3);
    }

    //
    // remap
    //
    map.remapTable(2);
    IS(map.size(), 4);
    IS(map.numKeys(), 3);
    OK(map.contains(1));
    OK(map.contains(4));
    OK(map.contains(700));
    OK(map.contains(1, 10));
    OK(map.contains(1, 2));
    OK(map.contains(4, 3));
    OK(map.contains(700, 1));
    OK(map.containsValue(10));
    OK(map.containsValue(2));
    OK(map.containsValue(3));
    OK(map.containsValue(1));
    IS(map.count(1), 2);
    IS(map.count(4), 1);
    IS(map.count(700), 1);

    //
    // iterate after rehash
    //
    {
        bool got_1_10 = false;
        bool got_1_2 = false;
        bool got_4_3 = false;
        bool got_700_1 = false;
        int n_passed = 0;

        MapType::Iterator it(map);
        OK("iterate");

        while (it.next())
        {
            n_passed++;
            if (it.key() == 1)
            {
                OK(it.hasContent());
                IS(it.values().size(), 2);

                switch (it.value())
                {
                case 2:
                    got_1_2 = true; break;
                case 10:
                    got_1_10 = true; break;
                default:
                    abort();
                }
            }
            else if (it.key() == 4)
            {
                OK(it.hasContent());
                IS(it.values().size(), 1);

                if (it.value() == 3)
                    got_4_3 = true;
                else
                    abort();
            }
            else if (it.key() == 700)
            {
                OK(it.hasContent());
                IS(it.values().size(), 1);

                if (it.value() == 700)
                    got_700_1 = true;
                else
                    abort();
            }
        }

        OK(got_1_10);
        OK(got_1_2);
        OK(got_4_3);
        OK(got_700_1);
        IS(n_passed, 4);
    }

    // tests for delete and clear
    OK("remove items");
    IS(map.remove(1), 2);
    IS(map.size(), 2);
    IS(map.numKeys(), 2);
    OK(!map.contains(1));
    OK(map.contains(4));
    OK(map.contains(700));

    {
        bool got_4_3 = false;
        bool got_700_1 = false;
        int n_passed = 0;

        MapType::Iterator it(map);
        OK("iterate");

        while (it.next())
        {
            n_passed++;
            if (it.key() == 4)
            {
                OK(it.hasContent());
                IS(it.values().size(), 1);

                if (it.value() == 3)
                    got_4_3 = true;
                else
                    abort();
            }
            else if (it.key() == 700)
            {
                OK(it.hasContent());
                IS(it.values().size(), 1);

                if (it.value() == 700)
                    got_700_1 = true;
                else
                    abort();
            }
            else
                abort();
        }

        OK(got_4_3);
        OK(got_700_1);
        IS(n_passed, 2);
    }

    //
    // clear
    //
    map.clear();
    OK("clear");
    IS(map.size(), 0);
    IS(map.numKeys(), 0);
    IS(map.numUsedBuckets(), 0);
    OK(!map.contains(1));
    OK(!map.contains(4));
    OK(!map.contains(700));
}
