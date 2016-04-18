#include "treecore/TestFramework.h"

#include "treecore/HashSet.h"
#include "treecore/String.h"

using namespace treecore;

typedef HashSet<String> StrSet;

void TestFramework::content()
{
    StrSet set;
    OK(set.insert("foo"));
    OK(set.insert("bar"));
    OK(!set.insert("foo"));
    OK(set.insert("baz"));
}
