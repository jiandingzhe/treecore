#include "TestFramework.h"

#include "treejuce/HashMap.h"

using namespace treejuce;

void TestFramework::content()
{
    HashMap<int, int> map;
    map.set(1, 2);
    IS(map[1], 2);
    IS(map.getReference(1), 2);

    map.getReference(1) = 3;
    IS(map[1], 3);

    IS(map.getReference(2), 0);
    OK(map.contains(2));
}
