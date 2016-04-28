#include "treecore/SortedSet.h"
#include "treecore/SimdFunc.h"
#include "treecore/TestFramework.h"

using namespace treecore;

void TestFramework::content( int argc, char** argv )
{
    SortedSet<SIMDType<16>, 16> set;
}
