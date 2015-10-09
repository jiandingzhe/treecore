#include "TestFramework.h"

#include "treecore/HashMap.h"
#include "treecore/String.h"

using namespace treecore;

typedef HashMap<String, String> MapType;

void TestFramework::content()
{
    MapType map;
    map.set("a", "b");
    OK(map.contains("a"));
    OK(map.containsValue("b"));
    IS(map["a"], "b");
    IS(map.size(), 1);
    IS(map.numUsedBuckets(), 1);


    map["a"] = "c";
    IS(map["a"], "c");
    OK(map.contains("a"));
    OK(map.containsValue("c"));
    IS(map.size(), 1);
    IS(map.numUsedBuckets(), 1);

    IS(map["d"], "");
    OK(map.contains("d"));
    IS(map.size(), 2);

    map["ccccc"] = "abcde";
    IS(map.size(), 3);
    OK(map.contains("ccccc"));
    IS(map["ccccc"], "abcde");

    // tests for const iterator
    {
        const MapType& mapref = map;

        {
            MapType::ConstIterator it(mapref);
            OK(!it.hasContent());

            bool got_a_c = false;
            bool got_d_ = false;
            bool got_ccccc_abcde = false;

            while (it.next())
            {
                OK(it.hasContent());
                if (it.key() == "a" && it.value() == "c")
                    got_a_c = true;
                else if (it.key() == "d" && it.value().length() == 0)
                    got_d_ = true;
                else if (it.key() == "ccccc" && it.value() == "abcde")
                    got_ccccc_abcde = true;
                else
                    abort();
            }

            OK(got_a_c);
            OK(got_d_);
            OK(got_ccccc_abcde);
        }

        {
            MapType::ConstIterator it(mapref);
            OK(mapref.select("ccccc", it));
            OK(it.hasContent());
            IS(it.key(), "ccccc");
            IS(it.value(), "abcde");
            IS(&map["ccccc"], &it.value());
        }
    }

    // tests for iterator
    {
        MapType::Iterator it(map);
        OK(!it.hasContent());

        OK(!map.insertOrSelect("ccccc", "ddddd", it));
        IS(map["ccccc"], "abcde");
        IS(map.size(), 3);
        OK(it.hasContent());
        IS(it.key(), "ccccc");
        IS(it.value(), "abcde");
        IS(&map["ccccc"], &it.value());

        OK(map.insertOrSelect("eeeee", "123", it));
        IS(map.size(), 4);
        OK(map.contains("eeeee"));
        OK(map.containsValue("123"));
        OK(it.hasContent());
        IS(it.key(), "eeeee");
        IS(it.value(), "123");

        OK(!map.select("wefew", it));
        IS(it.key(), "eeeee");
        IS(it.value(), "123");

        OK(map.select("ccccc", it));
        OK(it.hasContent());
        IS(it.key(), "ccccc");
        IS(it.value(), "abcde");
        IS(&map["ccccc"], &it.value());
    }

    // comparison operator test
    {
        MapType map2;
        map2["a"] = "c";
        map2["d"] = "";
        map2["ccccc"] = "abcde";
        map2["eeeee"] = "123";
        OK(map == map2);
    }

    {
        MapType map2;
        map2["a"] = "c";
        map2["d"] = "";
        map2["eeeee"] = "123";
        OK(map != map2);
    }

    {
        MapType map2;
        map2["a"] = "c";
        map2["d"] = "fooooo";
        map2["ccccc"] = "abcde";
        map2["eeeee"] = "123";
        OK(map != map2);
    }

    {
        MapType map2;
        map2["a"] = "c";
        map2["d"] = "";
        map2["ccfcc"] = "abcde";
        map2["eeeee"] = "123";
        OK(map != map2);
    }

    {
        MapType map2;
        map2["a"] = "c";
        map2["d"] = "";
        map2["ccccc"] = "abcde";
        map2["eeeee"] = "123";
        map2["foo"] = "bar";
        OK(map != map2);
    }


    // clear
    {
        map.clear();
        IS(map.size(), 0);
        IS(map.numUsedBuckets(), 0);
        OK(!map.contains("a"));
        OK(!map.contains("d"));
        OK(!map.contains("ccccc"));
        OK(!map.contains("eeeee"));
    }
}
