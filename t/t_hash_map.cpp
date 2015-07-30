#include "TestFramework.h"

#include "treecore/HashMap.h"
#include "treecore/String.h"

using namespace treecore;

void TestFramework::content()
{
    HashMap<String, String> map;
    map.set("a", "b");
    IS(map["a"], "b");
    IS(map.getReference("a"), "b");

    map.getReference("a") = "c";
    IS(map["a"], "c");

    IS(map.getReference("d"), "");
    OK(map.contains("d"));
}
