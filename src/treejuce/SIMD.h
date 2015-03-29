#ifndef TREEJUCE_SIMD_FUNC_H
#define TREEJUCE_SIMD_FUNC_H

#include "treejuce/Config.h"

#if defined TREEJUCE_CPU_X86
#  include "treejuce/SIMD/sse2.h"
#elif defined TREEJUCE_CPU_ARM
#  include "treejuce/SIMD/neon.h"
#else
#  error "SIMD function wrapper for CPU is not implemented"
#endif

#endif // TREEJUCE_SIMD_FUNC_H
