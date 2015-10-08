#include "TestFramework.h"

#include "treecore/HashMap.h"
#include "treecore/String.h"

using namespace treecore;

void TestFramework::content()
{
    HashMap<String, String> map;
    map.set("a", "b");
    OK(map.contains("a"));
    IS(map["a"], "b");

    map["a"] = "c";
    IS(map["a"], "c");

    IS(map["d"], "");
    OK(map.contains("d"));
}
