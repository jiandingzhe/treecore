#include "treecore/SIMD.h"
#include "TestFramework.h"

void TestFramework::content()
{
    OK(treecore::ScopedDenormalFlag::IsCpuCanDAZ());
}

