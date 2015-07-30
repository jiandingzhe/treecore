#include "TestFramework.h"

#include "treecore/HashMultiMap.h"

using namespace treecore;

void TestFramework::content()
{
    HashMultiMap<int, int> map;
    OK("map created");
    IS(map.size(), 0);

    map.insert(1, 10);
    OK("map insert value");
    IS(map.size(), 1);
    IS(map.getNumKeys(), 1);
    OK(map.contains(1));
    IS(map.count(1), 1);

    map.insert(1, 2);
    OK("map insert value");
    IS(map.size(), 2);
    IS(map.getNumKeys(), 1);
    OK(map.contains(1));
    IS(map.count(1), 2);

    map.insert(4, 3);
    OK("map insert value using another key");
    IS(map.size(), 3);
    IS(map.getNumKeys(), 2);
    OK(map.contains(1));
    OK(map.contains(4));
    IS(map.count(1), 2);
    IS(map.count(4), 1);

    map.insert(700, 1);
    OK("map insert values using yet another key");
    IS(map.size(), 4);
    IS(map.getNumKeys(), 3);
    OK(map.contains(1));
    OK(map.contains(4));
    OK(map.contains(700));
    IS(map.count(1), 2);
    IS(map.count(4), 1);
    IS(map.count(700), 1);

    {
        ArrayRef<int> values = map[1];
        OK("fetch values by key 1");
        IS(values.size(), 2);
        IS(values[0], 10);
        IS(values[1], 2);
    }

    {
        ArrayRef<int> values = map[4];
        OK("fetch values by key 4");
        IS(values.size(), 1);
        IS(values[0], 3);
    }

    {
        ArrayRef<int> values = map[700];
        OK("fetch values by key 700");
        IS(values.size(), 1);
        IS(values[0], 1);
    }

    //
    // iterate
    //
    {
        bool got_1_10 = false;
        bool got_1_2 = false;
        bool got_4_3 = false;
        bool got_700_1 = false;
        int n_got = 0;

        HashMultiMap<int,int>::Iterator it(map);
        OK("iterate");

        while (it.next())
        {
            int key = it.getKey();
            ArrayRef<int> values = it.getValues();

            if (key == 1)
                IS(values.size(), 2);
            else if (key == 4)
                IS(values.size(), 1);
            else if (key == 700)
                IS(values.size(), 1);

            for (int i_value = 0; i_value < values.size(); i_value++)
            {
                int value = values[i_value];
                n_got++;
                if (key == 1 && value == 10)
                    got_1_10 = true;
                else if (key == 1 && value == 2)
                    got_1_2 = true;
                else if (key == 4 && value == 3)
                    got_4_3 = true;
                else if (key == 700 && value == 1)
                    got_700_1 = true;
                else
                {
                    fprintf(stderr, "got unexpected: key %d value %d\n", key, value);
                }
            }
        }

        OK(got_1_10);
        OK(got_1_2);
        OK(got_4_3);
        OK(got_700_1);
        IS(n_got, 4);
    }

    //
    // remap
    //
    map.remapTable(2);
    OK("change number of slots");
    IS(map.getNumSlots(), 2);
    IS(map.size(), 4);
    IS(map.getNumKeys(), 3);
    OK(map.contains(1));
    OK(map.contains(4));
    OK(map.contains(700));
    IS(map.count(1), 2);
    IS(map.count(4), 1);
    IS(map.count(700), 1);

    {
        ArrayRef<int> values = map[1];
        OK("fetch values by key 1 after remap");
        IS(values.size(), 2);
        IS(values[0], 10);
        IS(values[1], 2);
    }

    {
        ArrayRef<int> values = map[4];
        OK("fetch values by key 4 after remap");
        IS(values.size(), 1);
        IS(values[0], 3);
    }

    {
        ArrayRef<int> values = map[700];
        OK("fetch values by key 700 after remap");
        IS(values.size(), 1);
        IS(values[0], 1);
    }

    //
    // iterate after rehash
    //
    {
        bool got_1_10 = false;
        bool got_1_2 = false;
        bool got_4_3 = false;
        bool got_700_1 = false;
        int n_got = 0;

        HashMultiMap<int,int>::Iterator it(map);
        OK("iterate after rehash");

        while (it.next())
        {
            int key = it.getKey();
            ArrayRef<int> values = it.getValues();

            if (key == 1)
                IS(values.size(), 2);
            else if (key == 4)
                IS(values.size(), 1);
            else if (key == 700)
                IS(values.size(), 1);

            for (int i_value = 0; i_value < values.size(); i_value++)
            {
                int value = values[i_value];
                n_got++;
                if (key == 1 && value == 10)
                    got_1_10 = true;
                else if (key == 1 && value == 2)
                    got_1_2 = true;
                else if (key == 4 && value == 3)
                    got_4_3 = true;
                else if (key == 700 && value == 1)
                    got_700_1 = true;
                else
                {
                    fprintf(stderr, "got unexpected: key %d value %d\n", key, value);
                }
            }
        }

        OK(got_1_10);
        OK(got_1_2);
        OK(got_4_3);
        OK(got_700_1);
        IS(n_got, 4);
    }

    // tests for delete and clear
    OK("remove items");
    IS(map.remove(1), 2);
    IS(map.size(), 2);
    IS(map.getNumKeys(), 2);
    OK(!map.contains(1));
    OK(map.contains(4));
    OK(map.contains(700));

    {
        bool got_4_3 = false;
        bool got_700_1 = false;
        int n_got = 0;

        HashMultiMap<int,int>::Iterator it(map);
        OK("iterate after remove");

        while (it.next())
        {
            int key = it.getKey();
            ArrayRef<int> values = it.getValues();

            for (int i = 0; i < values.size(); i++)
            {
                int value = values[i];
                n_got++;
                if (key == 4 && value == 3)
                    got_4_3 = true;
                else if (key == 700 && value == 1)
                    got_700_1 = true;
                else
                {
                    fprintf(stderr, "got unexpected: key %d value %d\n", key, value);
                }
            }
        }

        OK(got_4_3);
        OK(got_700_1);
        IS(n_got, 2);
    }

    //
    // clear
    //
    map.clear();
    OK("clear");
    IS(map.size(), 0);
    IS(map.getNumKeys(), 0);
    OK(!map.contains(1));
    OK(!map.contains(4));
    OK(!map.contains(700));
}
