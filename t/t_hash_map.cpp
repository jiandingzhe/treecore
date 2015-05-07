#include "TestFramework.h"

#include "treejuce/HashMap.h"
#include "treejuce/String.h"

using namespace treejuce;

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
