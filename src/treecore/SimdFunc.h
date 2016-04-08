#ifndef TREECORE_SIMD_FUNC_H
#define TREECORE_SIMD_FUNC_H

#include "treecore/Config.h"

#if TREECORE_CPU_X86
#  include "treecore/simd/sse2.h"
#elif TREECORE_CPU_ARM
#  include "treecore/simd/neon.h"
#else
#  error "SIMD function wrapper for CPU is not implemented"
#endif

#endif // TREECORE_SIMD_FUNC_H
