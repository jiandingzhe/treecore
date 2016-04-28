#include "treecore/Identifier.h"
#include "treecore/StringPool.h"

#include "treecore/TestFramework.h"

treecore::Identifier foo_global("foo");
treecore::Identifier bar_global("bar");

void TestFramework::content( int argc, char** argv )
{
    treecore::Identifier foo1("foo");
    treecore::Identifier baz1("baz");
    treecore::Identifier bar1("bar");
    treecore::Identifier bar2("bar");
    treecore::Identifier foo2("foo");
    treecore::Identifier bar3("bar");
    treecore::Identifier baz2("baz");

    treecore::Identifier foo_copy(foo1);
    treecore::Identifier baz_copy(baz1);

    IS(foo1.m_name, foo2.m_name);
    IS(foo1.m_name, foo_global.m_name);
    IS(foo1.m_name, foo_copy.m_name);

    IS(bar1.m_name, bar2.m_name);
    IS(bar1.m_name, bar3.m_name);
    IS(bar1.m_name, bar_global.m_name);

    IS(baz1.m_name, baz2.m_name);
    IS(baz1.m_name, baz_copy.m_name);

    OK(foo1.m_name != bar1.m_name);
    OK(foo1.m_name != baz1.m_name);
    OK(bar1.m_name != baz1.m_name);
}
