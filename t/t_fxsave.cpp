#include "treecore/SimdFunc.h"
#include "treecore/TestFramework.h"

void TestFramework::content()
{
    OK(treecore::ScopedDenormalFlag::IsCpuCanDAZ());
}

