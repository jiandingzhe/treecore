#include "treecore/TestFramework.h"
#include "treecore/IntTypes.h"

void TestFramework::content()
{
    IS(sizeof(treecore::int8), 1);
    IS(sizeof(treecore::int16), 2);
    IS(sizeof(treecore::int32), 4);
    IS(sizeof(treecore::int64), 8);

    IS(sizeof(treecore::uint8), 1);
    IS(sizeof(treecore::uint16), 2);
    IS(sizeof(treecore::uint32), 4);
    IS(sizeof(treecore::uint64), 8);
}
