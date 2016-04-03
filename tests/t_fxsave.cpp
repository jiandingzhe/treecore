#include "treecore/SimdFunc.h"
#include "TestFramework.h"

void TestFramework::content()
{
    OK(treecore::ScopedDenormalFlag::IsCpuCanDAZ());
}

