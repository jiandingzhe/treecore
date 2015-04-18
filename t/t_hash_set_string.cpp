#include "TestFramework.h"

#include "treejuce/HashSet.h"
#include "treejuce/String.h"

using namespace treejuce;

typedef HashSet<String> StrSet;

void TestFramework::content()
{
    StrSet set;
    OK(set.insert("foo"));
    OK(set.insert("bar"));
    OK(!set.insert("foo"));
    OK(set.insert("baz"));
}
