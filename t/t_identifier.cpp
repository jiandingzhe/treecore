#include "treecore/Identifier.h"
#include "TestFramework.h"

void TestFramework::content()
{
    treecore::Identifier foo1("foo");
    treecore::Identifier baz1("baz");
    treecore::Identifier bar1("bar");
    treecore::Identifier bar2("bar");
    treecore::Identifier foo2("foo");
    treecore::Identifier bar3("bar");
    treecore::Identifier baz2("baz");
    IS(foo1.m_name, foo2.m_name);
    IS(bar1.m_name, bar2.m_name);
    IS(bar1.m_name, bar3.m_name);
    IS(baz1.m_name, baz2.m_name);
    OK(foo1.m_name != bar1.m_name);
    OK(foo1.m_name != baz1.m_name);
    OK(bar1.m_name != baz1.m_name);
}
