#include "treecore/SIMD.h"

#include "TestFramework.h"

#include <cstdio>

void TestFramework::content()
{
    bool can_daz = treecore::ScopedDenormalFlag::IsCpuCanDAZ();
    std::printf("can daz: %d\n", can_daz);

    treecore::ScopedDenormalFlag();
    OK("scope object constructed");
}
