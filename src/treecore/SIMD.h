#ifndef TREECORE_SIMD_FUNC_H
#define TREECORE_SIMD_FUNC_H

#include "treecore/Config.h"

#if defined TREECORE_CPU_X86
#  include "treecore/SIMD/sse2.h"
#elif defined TREECORE_CPU_ARM
#  include "treecore/SIMD/neon.h"
#else
#  error "SIMD function wrapper for CPU is not implemented"
#endif

#endif // TREECORE_SIMD_FUNC_H
