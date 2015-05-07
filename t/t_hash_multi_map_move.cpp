#include "TestFramework.h"

#include "treejuce/HashMultiMap.h"
#include "treejuce/String.h"

using namespace treejuce;

void TestFramework::content()
{
    HashMultiMap<String, String> map1;
    map1.insert("a", "1");
    map1.insert("a", "2");
    map1.insert("b", "1");
    IS(map1.size(), 3);
    IS(map1.getNumKeys(), 2);

    HashMultiMap<String, String> map2(std::move(map1));
    IS(map1.size(), 0);
    IS(map1.getNumUsedSlots(), 0);
    IS(map1.getNumKeys(), 0);

    IS(map2.size(), 3);
    IS(map2.getNumKeys(), 2);
}
