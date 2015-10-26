#include "treecore/HashMap.h"
#include "TestFramework.h"

struct TestValue
{
    TestValue(int value): content(value) {}

    TestValue(const TestValue& other) = delete;
    TestValue(TestValue&& other): content(other.content)
    {
        other.content = 0;
    }

    TestValue& operator = (const TestValue& other) = delete;
    TestValue& operator = (TestValue&& other)
    {
        content = other.content;
        other.content = 0;
    }

    int content = 456;
};

typedef treecore::HashMap<int, TestValue> MapType;

void TestFramework::content()
{
    MapType map_tmp;

    MapType map_obj(std::move(map_tmp));
    OK("map move constructor compiles");

    {
        MapType::HashMapItem item_tmp{123, TestValue(789)};
        MapType::HashMapItem item(std::move(item_tmp));
        OK("map item move constructor compiles");
    }

    {
        TestValue to_be_moved(123);
        MapType::Iterator i(map_obj);

        map_obj.insertOrSelect(1, std::move(to_be_moved), i);
        IS(to_be_moved.content, 0);
        OK(map_obj.contains(1));

        OK(map_obj.select(1, i));
        IS(i.value().content, 123);
    }

    {
        TestValue to_be_moved(1111111);
        map_obj.set(66666, std::move(to_be_moved));
        IS(to_be_moved.content, 0);
        OK(map_obj.contains(66666));

        MapType::Iterator i(map_obj);
        OK(map_obj.select(66666, i));
        IS(i.value().content, 1111111);
    }
}