#include "treecore/TestFramework.h"

#include "treecore/HashMultiMap.h"
#include "treecore/String.h"

using namespace treecore;

void TestFramework::content( int argc, char** argv )
{
    HashMultiMap<String, String> map1;
    map1.store("a", "1");
    map1.store("a", "2");
    map1.store("b", "1");
    IS(map1.size(), 3);
    IS(map1.numKeys(), 2);

    HashMultiMap<String, String> map2(std::move(map1));
    IS(map1.size(), 0);
    IS(map1.numUsedBuckets(), 0);
    IS(map1.numKeys(), 0);

    IS(map2.size(), 3);
    IS(map2.numKeys(), 2);
}
