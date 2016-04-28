#include "treecore/TestFramework.h"
#include "treecore/HashSet.h"

using namespace treecore;

void TestFramework::content( int argc, char** argv )
{
    HashSet<int> set;

    OK("insert items");
    OK(set.insert(1));
    IS(set.size(), 1);
    OK(set.contains(1));

    OK(set.insert(2));
    IS(set.size(), 2);
    OK(set.contains(1));
    OK(set.contains(2));

    OK(!set.insert(1));
    IS(set.size(), 2);
    OK(set.contains(1));
    OK(set.contains(2));

    OK(set.insert(10));
    IS(set.size(), 3);
    OK(set.contains(1));
    OK(set.contains(2));
    OK(set.contains(10));


    // test for iterators
    {
        HashSet<int>::Iterator it(set);
        OK(!it.hasContent());
        OK(!set.insertOrSelect(10, it));
        OK(it.hasContent());
        IS(it.content(), 10);
        IS(set.size(), 3);
        OK(set.contains(1));
        OK(set.contains(2));
        OK(set.contains(10));
    }

    {
        HashSet<int>::Iterator it(set);
        OK(!it.hasContent());
        OK(set.insertOrSelect(17, it));
        OK(it.hasContent());
        IS(it.content(), 17);
        IS(set.size(), 4);
        OK(set.contains(1));
        OK(set.contains(2));
        OK(set.contains(10));
        OK(set.contains(17));
    }

    {
        HashSet<int>::Iterator it(set);
        OK(!it.hasContent());

        OK(!set.select(12345, it));
        OK(!it.hasContent());

        OK(set.select(2, it));
        OK(it.hasContent());
        IS(it.content(), 2);

        OK(!set.select(6789, it));
        OK(it.hasContent());
        IS(it.content(), 2);
    }

    OK("iterate through items");

    {
        HashSet<int>::Iterator it(set);
        bool got_1 = false;
        bool got_2 = false;
        bool got_10 = false;
        bool got_17 = false;
        int n_got = 0;

        while (it.next())
        {
            OK(it.hasContent());
            switch (it.content())
            {
            case 1:
                got_1 = true; n_got++; break;
            case 2:
                got_2 = true; n_got++; break;
            case 10:
                got_10 = true; n_got++; break;
            case 17:
                got_17 = true; n_got++; break;
            default:
                fprintf(stderr, "iterator got invalid value %d\n", it.content());
                abort();
            }
        }

        OK(got_1);
        OK(got_2);
        OK(got_10);
        OK(got_17);
        IS(n_got, 4);
    }

    set.remapTable(2);
    OK("rehash");
    IS(set.numBuckets(), 2);
    IS(set.size(), 4);
    OK(set.contains(1));
    OK(set.contains(2));
    OK(set.contains(10));
    OK(set.contains(17));

    OK("iterate after rehash");

    {
        HashSet<int>::Iterator it(set);
        bool got_1 = false;
        bool got_2 = false;
        bool got_10 = false;
        bool got_17 = false;
        int n_got = 0;

        while (it.next())
        {
            switch (it.content())
            {
            case 1:
                got_1 = true; n_got++; break;
            case 2:
                got_2 = true; n_got++; break;
            case 10:
                got_10 = true; n_got++; break;
            case 17:
                got_17 = true; n_got++; break;
            default:
                fprintf(stderr, "iterator got invalid value %d\n", it.content());
            }
        }

        OK(got_1);
        OK(got_2);
        OK(got_10);
        OK(got_17);
        IS(n_got, 4);
    }

    OK("remove non existing item");
    OK(!set.remove(100));
    IS(set.size(), 4);
    OK(set.contains(1));
    OK(set.contains(2));
    OK(set.contains(10));
    OK(set.contains(17));

    OK("remove items");
    OK(set.remove(2));
    IS(set.size(), 3);
    OK(set.contains(1));
    OK(!set.contains(2));
    OK(set.contains(10));
    OK(set.contains(17));

    OK(set.remove(17));
    IS(set.size(), 2);
    OK(set.contains(1));
    OK(!set.contains(2));
    OK(set.contains(10));
    OK(!set.contains(17));

    OK("iterate after removed some items");

    {
        HashSet<int>::Iterator it(set);
        bool got_1 = false;
        bool got_10 = false;
        int n_got = 0;

        while (it.next())
        {
            switch (it.content())
            {
            case 1:
                got_1 = true; n_got++; break;
            case 10:
                got_10 = true; n_got++; break;
            default:
                fprintf(stderr, "iterator got invalid value %d\n", it.content());
            }
        }

        OK(got_1);
        OK(got_10);
        IS(n_got, 2);
    }

    set.clear();
    OK("clear");
    IS(set.size(), 0);
    OK(!set.contains(1));
    OK(!set.contains(2));
    OK(!set.contains(10));
    OK(!set.contains(17));
}
