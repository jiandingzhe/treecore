#include "treecore/SimdFunc.h"
#include "treecore/TestFramework.h"

void TestFramework::content( int argc, char** argv )
{
    OK(treecore::ScopedDenormalFlag::IsCpuCanDAZ());
}

