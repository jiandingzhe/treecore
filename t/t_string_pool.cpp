#include "treecore/StringPool.h"
#include "treecore/TestFramework.h"

void TestFramework::content( int argc, char** argv )
{
    treecore::StringPool pool;
    IS(pool.m_strings.size(), 0);

    const char* foo = pool.getPooledString("foo");
    OK(foo);
    IS(pool.m_strings.size(), 1);

    const char* foo2 = pool.getPooledString("foo");
    OK(foo2);
    IS(pool.m_strings.size(), 1);
    IS(foo, foo2);

    const char* bar = pool.getPooledString("bar");
    OK(bar);
    IS(pool.m_strings.size(), 2);
    OK(foo != bar);

    const char* baz = pool.getPooledString("baz");
    OK(baz);
    IS(pool.m_strings.size(), 3);
    OK(foo != baz);
    OK(bar != baz);
    foo2 = pool.getPooledString("foo");
    IS(foo, foo2);

    // ensure string reliability after rehash
    pool.m_strings.remapTable(256);
    const char* foo_again = pool.getPooledString("foo");
    const char* bar_again = pool.getPooledString("bar");
    const char* baz_again = pool.getPooledString("baz");

    IS(foo, foo_again);
    IS(bar, bar_again);
    IS(baz, baz_again);
}
