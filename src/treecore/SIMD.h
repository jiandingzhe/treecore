#ifndef TREECORE_SIMD_FUNC_H
#define TREECORE_SIMD_FUNC_H

#include "treecore/IntTypes.h"
#include "treecore/ClassUtil.h"
#include "treecore/Memory.h"

#if defined TREECORE_CPU_X86
#  include "treecore/SIMD/sse2.h"
#elif defined TREECORE_CPU_ARM
#  include "treecore/SIMD/neon.h"
#else
#  error "SIMD function wrapper for CPU is not implemented"
#endif



namespace treecore
{

class ScopedDenormalFlag
{
public:
    TREECORE_STACK_CLASS(ScopedDenormalFlag);
    inline static bool IsCpuCanDAZ()
    {
        JUCE_ALIGN(64) uint8 data[512];
        checkPtrSIMD(&data,64);
        zeromem(data,512);
#if defined TREECORE_COMPILER_MSVC && TREECORE_SIZE_PTR == 4
        int* h=(int*)&data;
        __asm {
            mov esi,[h]
            fxsave [esi]
        }
#elif defined TREECORE_COMPILER_MSVC && TREECORE_SIZE_PTR == 8
        _fxsave(data);
#elif defined TREECORE_COMPILER_GCC || defined TREECORE_COMPILER_CLANG
        __builtin_ia32_fxsave(data);
#else
#  error unsupported platform!
#endif
        uint32 f=*(uint32*)&data[28];
        const uint32 k=32;
        f = f&k;
        return f==32;
    }

    enum Rounding
    {
        ROUND_NEAREST = 0,
        ROUND_NEGATIVE,
        ROUND_POSITIVE,
        ROUND_TO_ZERO
    };

    forcedinline ScopedDenormalFlag(Rounding mode = ROUND_NEAREST)
        :sse_control_store(_mm_getcsr())
        ,needProcess()
    {
        const unsigned int k = (0x9fc0u | (mode << 13));
        if (sse_control_store!=k)
        {
            _mm_setcsr(k);
            needProcess=true;
        }
        else
        {
            needProcess=false;
        }
    };

    forcedinline ~ScopedDenormalFlag()
    {
        if (needProcess) {
            _mm_setcsr(sse_control_store);
        }
    };

private:
    const unsigned int sse_control_store;
    bool needProcess;

    TREECORE_DECLARE_NON_COPYABLE(ScopedDenormalFlag);
};

} // namespace treecore

#endif // TREECORE_SIMD_FUNC_H
