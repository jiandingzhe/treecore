#ifndef TREECORE_SIMD_FUNC_SSE2_H
#define TREECORE_SIMD_FUNC_SSE2_H

#include "treecore/simd/template.h"

#include "treecore/IntTypes.h"
#include "treecore/ClassUtil.h"
#include "treecore/Memory.h"

#include <cstdint>
#include <emmintrin.h>
#include <immintrin.h>

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
#elif defined TREECORE_COMPILER_GCC
#  if defined TREECORE_OS_LINUX
        __builtin_ia32_fxsave(data);
#  else
        __asm__ __volatile__(
                            " fxsave %0;"
                            :"=m"(data)
                            :
                            :
                            );
#  endif
#elif defined TREECORE_COMPILER_CLANG
        __asm__ __volatile__(
                            " fxsave %0;"
                            :"=m"(data)
                            :
                            :
                            );
#else
#  error fxsave in this platform??
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

/************************************************************************
* 向全部缓存刷新内存地址
************************************************************************/
forcedinline void MemoryFetch(const void*const ptr)
{
    _mm_prefetch((const char*)ptr,_MM_HINT_NTA);
}

/************************************************************************
* 向T2缓存刷新内存地址
************************************************************************/
forcedinline void MemoryFetchT2(const void*const ptr)
{
    _mm_prefetch((const char*)ptr,_MM_HINT_T2);
}

/************************************************************************
* 向T1缓存刷新内存地址
************************************************************************/
forcedinline void MemoryFetchT1(const void*const ptr)
{
    _mm_prefetch((const char*)ptr,_MM_HINT_T1);
}

/************************************************************************
* 向T0缓存刷新内存地址
************************************************************************/
forcedinline void MemoryFetchT0(const void*const ptr)
{
    _mm_prefetch((const char*)ptr,_MM_HINT_T0);
}

//
// 16-byte SSE operations
//
template<>
union SIMDType<16>
{
    __m128  simd_by_float;
    __m128i simd_by_int;
    __m128d simd_by_double;
    int8   values_by_i8[16];
    int16   values_by_i16[8];
    int32   values_by_i32[4];
    int64   values_by_i64[2];
    float   values_by_float[4];
    double  values_by_double[2];
};

static_assert(sizeof(SIMDType<16>) == 16, "SIMDType<16> is 16 byte");

// get one component
template<> inline int8 simd_get_one<0,  int8, 16> (const SIMDType<16>& data) noexcept { return data.values_by_i8[0]; }
template<> inline int8 simd_get_one<1,  int8, 16> (const SIMDType<16>& data) noexcept { return data.values_by_i8[1]; }
template<> inline int8 simd_get_one<2,  int8, 16> (const SIMDType<16>& data) noexcept { return data.values_by_i8[2]; }
template<> inline int8 simd_get_one<3,  int8, 16> (const SIMDType<16>& data) noexcept { return data.values_by_i8[3]; }
template<> inline int8 simd_get_one<4,  int8, 16> (const SIMDType<16>& data) noexcept { return data.values_by_i8[4]; }
template<> inline int8 simd_get_one<5,  int8, 16> (const SIMDType<16>& data) noexcept { return data.values_by_i8[5]; }
template<> inline int8 simd_get_one<6,  int8, 16> (const SIMDType<16>& data) noexcept { return data.values_by_i8[6]; }
template<> inline int8 simd_get_one<7,  int8, 16> (const SIMDType<16>& data) noexcept { return data.values_by_i8[7]; }
template<> inline int8 simd_get_one<8,  int8, 16> (const SIMDType<16>& data) noexcept { return data.values_by_i8[8]; }
template<> inline int8 simd_get_one<9,  int8, 16> (const SIMDType<16>& data) noexcept { return data.values_by_i8[9]; }
template<> inline int8 simd_get_one<10, int8, 16> (const SIMDType<16>& data) noexcept { return data.values_by_i8[10]; }
template<> inline int8 simd_get_one<11, int8, 16> (const SIMDType<16>& data) noexcept { return data.values_by_i8[11]; }
template<> inline int8 simd_get_one<12, int8, 16> (const SIMDType<16>& data) noexcept { return data.values_by_i8[12]; }
template<> inline int8 simd_get_one<13, int8, 16> (const SIMDType<16>& data) noexcept { return data.values_by_i8[13]; }
template<> inline int8 simd_get_one<14, int8, 16> (const SIMDType<16>& data) noexcept { return data.values_by_i8[14]; }
template<> inline int8 simd_get_one<15, int8, 16> (const SIMDType<16>& data) noexcept { return data.values_by_i8[15]; }

template<> inline int16 simd_get_one<0, int16, 16> (const SIMDType<16>& data) noexcept { return data.values_by_i16[0]; }
template<> inline int16 simd_get_one<1, int16, 16> (const SIMDType<16>& data) noexcept { return data.values_by_i16[1]; }
template<> inline int16 simd_get_one<2, int16, 16> (const SIMDType<16>& data) noexcept { return data.values_by_i16[2]; }
template<> inline int16 simd_get_one<3, int16, 16> (const SIMDType<16>& data) noexcept { return data.values_by_i16[3]; }
template<> inline int16 simd_get_one<4, int16, 16> (const SIMDType<16>& data) noexcept { return data.values_by_i16[4]; }
template<> inline int16 simd_get_one<5, int16, 16> (const SIMDType<16>& data) noexcept { return data.values_by_i16[5]; }
template<> inline int16 simd_get_one<6, int16, 16> (const SIMDType<16>& data) noexcept { return data.values_by_i16[6]; }
template<> inline int16 simd_get_one<7, int16, 16> (const SIMDType<16>& data) noexcept { return data.values_by_i16[7]; }

template<> inline int32 simd_get_one<0, int32, 16> (const SIMDType<16>& data) noexcept { return data.values_by_i32[0]; }
template<> inline int32 simd_get_one<1, int32, 16> (const SIMDType<16>& data) noexcept { return data.values_by_i32[1]; }
template<> inline int32 simd_get_one<2, int32, 16> (const SIMDType<16>& data) noexcept { return data.values_by_i32[2]; }
template<> inline int32 simd_get_one<3, int32, 16> (const SIMDType<16>& data) noexcept { return data.values_by_i32[3]; }

template<> inline float simd_get_one<0, float, 16> (const SIMDType<16>& data) noexcept { return data.values_by_float[0]; }
template<> inline float simd_get_one<1, float, 16> (const SIMDType<16>& data) noexcept { return data.values_by_float[1]; }
template<> inline float simd_get_one<2, float, 16> (const SIMDType<16>& data) noexcept { return data.values_by_float[2]; }
template<> inline float simd_get_one<3, float, 16> (const SIMDType<16>& data) noexcept { return data.values_by_float[3]; }

template<> inline double simd_get_one<0, double, 16> (const SIMDType<16>& data) noexcept { return data.values_by_double[0]; }
template<> inline double simd_get_one<1, double, 16> (const SIMDType<16>& data) noexcept { return data.values_by_double[1]; }

// set one component
template<> inline void simd_set_one <0,  int8, 16> (SIMDType<16>& target, int8 value) noexcept { target.values_by_i8[0] = value; }
template<> inline void simd_set_one <1,  int8, 16> (SIMDType<16>& target, int8 value) noexcept { target.values_by_i8[1] = value; }
template<> inline void simd_set_one <2,  int8, 16> (SIMDType<16>& target, int8 value) noexcept { target.values_by_i8[2] = value; }
template<> inline void simd_set_one <3,  int8, 16> (SIMDType<16>& target, int8 value) noexcept { target.values_by_i8[3] = value; }
template<> inline void simd_set_one <4,  int8, 16> (SIMDType<16>& target, int8 value) noexcept { target.values_by_i8[4] = value; }
template<> inline void simd_set_one <5,  int8, 16> (SIMDType<16>& target, int8 value) noexcept { target.values_by_i8[5] = value; }
template<> inline void simd_set_one <6,  int8, 16> (SIMDType<16>& target, int8 value) noexcept { target.values_by_i8[6] = value; }
template<> inline void simd_set_one <7,  int8, 16> (SIMDType<16>& target, int8 value) noexcept { target.values_by_i8[7] = value; }
template<> inline void simd_set_one <8,  int8, 16> (SIMDType<16>& target, int8 value) noexcept { target.values_by_i8[8] = value; }
template<> inline void simd_set_one <9,  int8, 16> (SIMDType<16>& target, int8 value) noexcept { target.values_by_i8[9] = value; }
template<> inline void simd_set_one <10, int8, 16> (SIMDType<16>& target, int8 value) noexcept { target.values_by_i8[10] = value; }
template<> inline void simd_set_one <11, int8, 16> (SIMDType<16>& target, int8 value) noexcept { target.values_by_i8[11] = value; }
template<> inline void simd_set_one <12, int8, 16> (SIMDType<16>& target, int8 value) noexcept { target.values_by_i8[12] = value; }
template<> inline void simd_set_one <13, int8, 16> (SIMDType<16>& target, int8 value) noexcept { target.values_by_i8[13] = value; }
template<> inline void simd_set_one <14, int8, 16> (SIMDType<16>& target, int8 value) noexcept { target.values_by_i8[14] = value; }
template<> inline void simd_set_one <15, int8, 16> (SIMDType<16>& target, int8 value) noexcept { target.values_by_i8[15] = value; }

template<> inline void simd_set_one <0, int16, 16> (SIMDType<16>& target, int16 value) noexcept { target.values_by_i16[0] = value; }
template<> inline void simd_set_one <1, int16, 16> (SIMDType<16>& target, int16 value) noexcept { target.values_by_i16[1] = value; }
template<> inline void simd_set_one <2, int16, 16> (SIMDType<16>& target, int16 value) noexcept { target.values_by_i16[2] = value; }
template<> inline void simd_set_one <3, int16, 16> (SIMDType<16>& target, int16 value) noexcept { target.values_by_i16[3] = value; }
template<> inline void simd_set_one <4, int16, 16> (SIMDType<16>& target, int16 value) noexcept { target.values_by_i16[4] = value; }
template<> inline void simd_set_one <5, int16, 16> (SIMDType<16>& target, int16 value) noexcept { target.values_by_i16[5] = value; }
template<> inline void simd_set_one <6, int16, 16> (SIMDType<16>& target, int16 value) noexcept { target.values_by_i16[6] = value; }
template<> inline void simd_set_one <7, int16, 16> (SIMDType<16>& target, int16 value) noexcept { target.values_by_i16[7] = value; }

template<> inline void simd_set_one <0, int32, 16> (SIMDType<16>& target, int32 value) noexcept { target.values_by_i32[0] = value; }
template<> inline void simd_set_one <1, int32, 16> (SIMDType<16>& target, int32 value) noexcept { target.values_by_i32[1] = value; }
template<> inline void simd_set_one <2, int32, 16> (SIMDType<16>& target, int32 value) noexcept { target.values_by_i32[2] = value; }
template<> inline void simd_set_one <3, int32, 16> (SIMDType<16>& target, int32 value) noexcept { target.values_by_i32[3] = value; }

template<> inline void simd_set_one <0, float, 16> (SIMDType<16>& target, float value) noexcept { target.values_by_float[0] = value; }
template<> inline void simd_set_one <1, float, 16> (SIMDType<16>& target, float value) noexcept { target.values_by_float[1] = value; }
template<> inline void simd_set_one <2, float, 16> (SIMDType<16>& target, float value) noexcept { target.values_by_float[2] = value; }
template<> inline void simd_set_one <3, float, 16> (SIMDType<16>& target, float value) noexcept { target.values_by_float[3] = value; }

template<> inline void simd_set_one <0, double, 16> (SIMDType<16>& target, double value) noexcept { target.values_by_double[0] = value; }
template<> inline void simd_set_one <1, double, 16> (SIMDType<16>& target, double value) noexcept { target.values_by_double[1] = value; }

template<> inline void simd_get_all<int32,  16> (const SIMDType<16>& target, int32*  store) noexcept { _mm_store_si128((__m128i*)store, target.simd_by_int); }
template<> inline void simd_get_all<float,  16> (const SIMDType<16>& target, float*  store) noexcept { _mm_store_ps(store, target.simd_by_float); }
template<> inline void simd_get_all<double, 16> (const SIMDType<16>& target, double* store) noexcept { _mm_store_pd(store, target.simd_by_double); }

// set all components by specifying all of them
template<> inline void simd_set_all<int16,  16> (SIMDType<16>& target, int16 a, int16 b, int16 c, int16 d, int16 e, int16 f, int16 g, int16 h) noexcept { target.simd_by_int = _mm_set_epi16(h, g, f, e, d, c, b, a); }
template<> inline void simd_set_all<int32,  16> (SIMDType<16>& target, int32 a, int32 b, int32 c, int32 d) noexcept { target.simd_by_int    = _mm_set_epi32(d, c, b, a); }
template<> inline void simd_set_all<float,  16> (SIMDType<16>& target, float a, float b, float c, float d) noexcept { target.simd_by_float  = _mm_set_ps(d, c, b, a);    }
template<> inline void simd_set_all<double, 16> (SIMDType<16>& target, double a, double b)                 noexcept { target.simd_by_double = _mm_set_pd(b, a);          }

template<> inline void simd_set_all<int32,  16> (SIMDType<16>& target, const int32* values)  noexcept { target.simd_by_int    = _mm_loadu_si128((const __m128i*)values); }
template<> inline void simd_set_all<float,  16> (SIMDType<16>& target, const float* values)  noexcept { target.simd_by_float  = _mm_loadu_ps(values);                    }
template<> inline void simd_set_all<double, 16> (SIMDType<16>& target, const double* values) noexcept { target.simd_by_double = _mm_loadu_pd(values);                    }

// set all components using one same value
template<> inline void simd_broadcast<int32,  16> (SIMDType<16>& target, int32 value)  noexcept  { target.simd_by_int    = _mm_set1_epi32(value); }
template<> inline void simd_broadcast<float,  16> (SIMDType<16>& target, float value)  noexcept  { target.simd_by_float  = _mm_set1_ps(value);    }
template<> inline void simd_broadcast<double, 16> (SIMDType<16>& target, double value) noexcept  { target.simd_by_double = _mm_set1_pd(value);    }

// arithmetical and logical operations
template<> inline void simd_add<int32,  16> (SIMDType<16>& target, const SIMDType<16>& a, const SIMDType<16>& b) noexcept { target.simd_by_int    = _mm_add_epi32(a.simd_by_int,    b.simd_by_int); }
template<> inline void simd_add<float,  16> (SIMDType<16>& target, const SIMDType<16>& a, const SIMDType<16>& b) noexcept { target.simd_by_float  = _mm_add_ps   (a.simd_by_float,  b.simd_by_float); }
template<> inline void simd_add<double, 16> (SIMDType<16>& target, const SIMDType<16>& a, const SIMDType<16>& b) noexcept { target.simd_by_double = _mm_add_pd   (a.simd_by_double, b.simd_by_double); }

template<> inline void simd_sub<int32,  16> (SIMDType<16>& target, const SIMDType<16>& a, const SIMDType<16>& b) noexcept { target.simd_by_int    = _mm_sub_epi32(a.simd_by_int,    b.simd_by_int); }
template<> inline void simd_sub<float,  16> (SIMDType<16>& target, const SIMDType<16>& a, const SIMDType<16>& b) noexcept { target.simd_by_float  = _mm_sub_ps   (a.simd_by_float,  b.simd_by_float); }
template<> inline void simd_sub<double, 16> (SIMDType<16>& target, const SIMDType<16>& a, const SIMDType<16>& b) noexcept { target.simd_by_double = _mm_sub_pd   (a.simd_by_double, b.simd_by_double); }

template<> inline void simd_mul<uint32, 16> (SIMDType<16>& target, const SIMDType<16>& a, const SIMDType<16>& b) noexcept { target.simd_by_int    = _mm_mul_epu32(a.simd_by_int,    b.simd_by_int);      }
template<> inline void simd_mul<float,  16> (SIMDType<16>& target, const SIMDType<16>& a, const SIMDType<16>& b) noexcept { target.simd_by_float  = _mm_mul_ps   (a.simd_by_float,  b.simd_by_float);     }
template<> inline void simd_mul<double, 16> (SIMDType<16>& target, const SIMDType<16>& a, const SIMDType<16>& b) noexcept { target.simd_by_double = _mm_mul_pd   (a.simd_by_double, b.simd_by_double);   }

template<> inline void simd_div<float,  16> (SIMDType<16>& target, const SIMDType<16>& a, const SIMDType<16>& b) noexcept { target.simd_by_float  = _mm_div_ps(a.simd_by_float,  b.simd_by_float);     }
template<> inline void simd_div<double, 16> (SIMDType<16>& target, const SIMDType<16>& a, const SIMDType<16>& b) noexcept { target.simd_by_double = _mm_div_pd(a.simd_by_double, b.simd_by_double);   }

template<> inline void simd_cmpl<float, 16>  (SIMDType<16>& target) noexcept
{
    SIMDType<16> ones;
    simd_broadcast<int32>(ones, 0xffffffff);
    target.simd_by_float = _mm_andnot_ps(target.simd_by_float, ones.simd_by_float);
}

template<> inline void simd_cmpl<double, 16>  (SIMDType<16>& target) noexcept
{
    SIMDType<16> ones;
    simd_broadcast<int32>(ones, 0xffffffff);
    target.simd_by_double = _mm_andnot_pd(target.simd_by_double, ones.simd_by_double);
}

template<> inline void simd_cmpl<int, 16>  (SIMDType<16>& target) noexcept
{
    SIMDType<16> ones;
    simd_broadcast<int32>(ones, 0xffffffff);
    target.simd_by_int = _mm_andnot_si128(target.simd_by_int, ones.simd_by_int);
}

template<> inline void simd_and<int32,  16> (SIMDType<16>& target, const SIMDType<16>& a, const SIMDType<16>& b) noexcept { target.simd_by_int    = _mm_and_si128(a.simd_by_int,    b.simd_by_int);      }
template<> inline void simd_and<float,  16> (SIMDType<16>& target, const SIMDType<16>& a, const SIMDType<16>& b) noexcept { target.simd_by_float  = _mm_and_ps   (a.simd_by_float,  b.simd_by_float);     }
template<> inline void simd_and<double, 16> (SIMDType<16>& target, const SIMDType<16>& a, const SIMDType<16>& b) noexcept { target.simd_by_double = _mm_and_pd   (a.simd_by_double, b.simd_by_double);   }

template<> inline void simd_or<int32,   16> (SIMDType<16>& target, const SIMDType<16>& a, const SIMDType<16>& b) noexcept { target.simd_by_int    = _mm_or_si128(a.simd_by_int,    b.simd_by_int);       }
template<> inline void simd_or<float,   16> (SIMDType<16>& target, const SIMDType<16>& a, const SIMDType<16>& b) noexcept { target.simd_by_float  = _mm_or_ps   (a.simd_by_float,  b.simd_by_float);      }
template<> inline void simd_or<double,  16> (SIMDType<16>& target, const SIMDType<16>& a, const SIMDType<16>& b) noexcept { target.simd_by_double = _mm_or_pd   (a.simd_by_double, b.simd_by_double);    }

template<> inline void simd_xor<float,  16> (SIMDType<16>& target, const SIMDType<16>& a, const SIMDType<16>& b) noexcept { target.simd_by_float  = _mm_xor_ps   (a.simd_by_float,  b.simd_by_float);     }
template<> inline void simd_xor<int32,  16> (SIMDType<16>& target, const SIMDType<16>& a, const SIMDType<16>& b) noexcept { target.simd_by_int    = _mm_xor_si128(a.simd_by_int,    b.simd_by_int);      }
template<> inline void simd_xor<double, 16> (SIMDType<16>& target, const SIMDType<16>& a, const SIMDType<16>& b) noexcept { target.simd_by_double = _mm_xor_pd   (a.simd_by_double, b.simd_by_double);   }

template<> inline void simd_nand<int32,  16> (SIMDType<16>& target, const SIMDType<16>& a, const SIMDType<16>& b) noexcept { target.simd_by_int    = _mm_andnot_si128(a.simd_by_int,    b.simd_by_int);      }
template<> inline void simd_nand<float,  16> (SIMDType<16>& target, const SIMDType<16>& a, const SIMDType<16>& b) noexcept { target.simd_by_float  = _mm_andnot_ps   (a.simd_by_float,  b.simd_by_float);     }
template<> inline void simd_nand<double, 16> (SIMDType<16>& target, const SIMDType<16>& a, const SIMDType<16>& b) noexcept { target.simd_by_double = _mm_andnot_pd   (a.simd_by_double, b.simd_by_double);   }

template<> inline void simd_cmp<int8,   16> (SIMDType<16>& target, const SIMDType<16>& a, const SIMDType<16>& b) noexcept { target.simd_by_int    = _mm_cmpeq_epi8 (a.simd_by_int,    b.simd_by_int);    }
template<> inline void simd_cmp<int16,  16> (SIMDType<16>& target, const SIMDType<16>& a, const SIMDType<16>& b) noexcept { target.simd_by_int    = _mm_cmpeq_epi16(a.simd_by_int,    b.simd_by_int);    }
template<> inline void simd_cmp<int32,  16> (SIMDType<16>& target, const SIMDType<16>& a, const SIMDType<16>& b) noexcept { target.simd_by_int    = _mm_cmpeq_epi32(a.simd_by_int,    b.simd_by_int);    }
template<> inline void simd_cmp<float,  16> (SIMDType<16>& target, const SIMDType<16>& a, const SIMDType<16>& b) noexcept { target.simd_by_float  = _mm_cmpeq_ps   (a.simd_by_float,  b.simd_by_float);   }
template<> inline void simd_cmp<double, 16> (SIMDType<16>& target, const SIMDType<16>& a, const SIMDType<16>& b) noexcept { target.simd_by_double = _mm_cmpeq_pd   (a.simd_by_double, b.simd_by_double); }

template<> inline void simd_gt<int8,   16> (SIMDType<16>& target, const SIMDType<16>& a, const SIMDType<16>& b) noexcept { target.simd_by_int    = _mm_cmpgt_epi8 (a.simd_by_int,    b.simd_by_int);    }
template<> inline void simd_gt<int16,  16> (SIMDType<16>& target, const SIMDType<16>& a, const SIMDType<16>& b) noexcept { target.simd_by_int    = _mm_cmpgt_epi16(a.simd_by_int,    b.simd_by_int);    }
template<> inline void simd_gt<int32,  16> (SIMDType<16>& target, const SIMDType<16>& a, const SIMDType<16>& b) noexcept { target.simd_by_int    = _mm_cmpgt_epi32(a.simd_by_int,    b.simd_by_int);    }
template<> inline void simd_gt<float,  16> (SIMDType<16>& target, const SIMDType<16>& a, const SIMDType<16>& b) noexcept { target.simd_by_float  = _mm_cmpgt_ps   (a.simd_by_float,  b.simd_by_float);   }
template<> inline void simd_gt<double, 16> (SIMDType<16>& target, const SIMDType<16>& a, const SIMDType<16>& b) noexcept { target.simd_by_double = _mm_cmpgt_pd   (a.simd_by_double, b.simd_by_double); }

template<> inline void simd_ge<float,  16> (SIMDType<16>& target, const SIMDType<16>& a, const SIMDType<16>& b) noexcept { target.simd_by_float  = _mm_cmpge_ps(a.simd_by_float,  b.simd_by_float);  }
template<> inline void simd_ge<double, 16> (SIMDType<16>& target, const SIMDType<16>& a, const SIMDType<16>& b) noexcept { target.simd_by_double = _mm_cmpge_pd(a.simd_by_double, b.simd_by_double); }

template<> inline void simd_lt<int8,   16> (SIMDType<16>& target, const SIMDType<16>& a, const SIMDType<16>& b) noexcept { target.simd_by_int    = _mm_cmplt_epi8 (a.simd_by_int,    b.simd_by_int);    }
template<> inline void simd_lt<int16,  16> (SIMDType<16>& target, const SIMDType<16>& a, const SIMDType<16>& b) noexcept { target.simd_by_int    = _mm_cmplt_epi16(a.simd_by_int,    b.simd_by_int);    }
template<> inline void simd_lt<int32,  16> (SIMDType<16>& target, const SIMDType<16>& a, const SIMDType<16>& b) noexcept { target.simd_by_int    = _mm_cmplt_epi32(a.simd_by_int,    b.simd_by_int);    }
template<> inline void simd_lt<float,  16> (SIMDType<16>& target, const SIMDType<16>& a, const SIMDType<16>& b) noexcept { target.simd_by_float  = _mm_cmplt_ps   (a.simd_by_float,  b.simd_by_float);  }
template<> inline void simd_lt<double, 16> (SIMDType<16>& target, const SIMDType<16>& a, const SIMDType<16>& b) noexcept { target.simd_by_double = _mm_cmplt_pd   (a.simd_by_double, b.simd_by_double); }

template<> inline void simd_le<float,  16> (SIMDType<16>& target, const SIMDType<16>& a, const SIMDType<16>& b) noexcept { target.simd_by_float  = _mm_cmple_ps(a.simd_by_float,  b.simd_by_float);  }
template<> inline void simd_le<double, 16> (SIMDType<16>& target, const SIMDType<16>& a, const SIMDType<16>& b) noexcept { target.simd_by_double = _mm_cmple_pd(a.simd_by_double, b.simd_by_double); }

// directly compare two int64 values for integer equality test
template<> inline bool simd_equal<int8, 16>(const SIMDType<16>& a, const SIMDType<16>& b) noexcept
{
    return a.values_by_i64[0] == b.values_by_i64[0] &&
            a.values_by_i64[1] == b.values_by_i64[1];
}

template<> inline bool simd_equal<int16, 16>(const SIMDType<16>& a, const SIMDType<16>& b) noexcept
{
    return a.values_by_i64[0] == b.values_by_i64[0] &&
            a.values_by_i64[1] == b.values_by_i64[1];
}

template<> inline bool simd_equal<int32, 16>(const SIMDType<16>& a, const SIMDType<16>& b) noexcept
{
    return a.values_by_i64[0] == b.values_by_i64[0] &&
            a.values_by_i64[1] == b.values_by_i64[1];
}

template<> inline bool simd_equal<int64, 16>(const SIMDType<16>& a, const SIMDType<16>& b) noexcept
{
    return a.values_by_i64[0] == b.values_by_i64[0] &&
            a.values_by_i64[1] == b.values_by_i64[1];
}

// use SIMD instruction for float equality test
template<> inline bool simd_equal<float, 16>(const SIMDType<16>& a, const SIMDType<16>& b) noexcept
{
    SIMDType<16> result;
    simd_cmp<float>(result, a, b);

    return bool(result.values_by_i32[0]) &&
            bool(result.values_by_i32[1]) &&
            bool(result.values_by_i32[2]) &&
            bool(result.values_by_i32[3]);
}

template<> inline bool simd_equal<double, 16>(const SIMDType<16>& a, const SIMDType<16>& b) noexcept
{
    SIMDType<16> result;
    simd_cmp<double>(result, a, b);

    return bool(result.values_by_i64[0]) &&
            bool(result.values_by_i64[1]);
}

// reorder
template<int IDX1, int IDX2, int IDX3, int IDX4, int SZ>
struct _simd_shuffle_impl_;

// inspired by KEMeng's implementation
template<int IDX1, int IDX2, int IDX3, int IDX4>
struct _simd_shuffle_impl_<IDX1, IDX2, IDX3, IDX4, 16>
{
    static void shuffle(SIMDType<16>& target)
    {
#ifdef _MSC_VER
        const int control = IDX1 + (IDX2 << 2) + (IDX3 << 4) + (IDX4 << 6);
#else
        constexpr int control = IDX1 + (IDX2 << 2) + (IDX3 << 4) + (IDX4 << 6);
#endif

        target.simd_by_int = _mm_shuffle_epi32(target.simd_by_int, control);
    }
};

template<int IDX1, int IDX2, int IDX3, int IDX4, int SZ>
inline void simd_shuffle(SIMDType<SZ>& target) noexcept
{
    _simd_shuffle_impl_<IDX1, IDX2, IDX3, IDX4, SZ>::shuffle(target);
}

template<>
inline float simd_sum<float, 16>(const SIMDType<16>& value) noexcept
{
    SIMDType<16> tmp;
    tmp.simd_by_float = _mm_hadd_ps(value.simd_by_float, value.simd_by_float);
    tmp.simd_by_float = _mm_hadd_ps(tmp.simd_by_float, tmp.simd_by_float);
    return tmp.values_by_float[0];
}

//
// 64 bit SIMD operations
//

#if ! (defined TREECORE_COMPILER_MSVC && TREECORE_SIZE_PTR == 8)

template<>
union SIMDType<8>
{
    __m64 simd_value;
    int8  values_by_i8[8];
    int16 values_by_i16[4];
    int32 values_by_i32[2];
    int64 value_by_i64;
};

// get one component
template<> inline int8  simd_get_one<0, int8,  8> (const SIMDType<8>& data) noexcept { return data.values_by_i8[0]; }
template<> inline int8  simd_get_one<1, int8,  8> (const SIMDType<8>& data) noexcept { return data.values_by_i8[1]; }
template<> inline int8  simd_get_one<2, int8,  8> (const SIMDType<8>& data) noexcept { return data.values_by_i8[2]; }
template<> inline int8  simd_get_one<3, int8,  8> (const SIMDType<8>& data) noexcept { return data.values_by_i8[3]; }
template<> inline int8  simd_get_one<4, int8,  8> (const SIMDType<8>& data) noexcept { return data.values_by_i8[4]; }
template<> inline int8  simd_get_one<5, int8,  8> (const SIMDType<8>& data) noexcept { return data.values_by_i8[5]; }
template<> inline int8  simd_get_one<6, int8,  8> (const SIMDType<8>& data) noexcept { return data.values_by_i8[6]; }
template<> inline int8  simd_get_one<7, int8,  8> (const SIMDType<8>& data) noexcept { return data.values_by_i8[7]; }

template<> inline int16 simd_get_one<0, int16, 8> (const SIMDType<8>& data) noexcept { return data.values_by_i16[0]; }
template<> inline int16 simd_get_one<1, int16, 8> (const SIMDType<8>& data) noexcept { return data.values_by_i16[1]; }
template<> inline int16 simd_get_one<2, int16, 8> (const SIMDType<8>& data) noexcept { return data.values_by_i16[2]; }
template<> inline int16 simd_get_one<3, int16, 8> (const SIMDType<8>& data) noexcept { return data.values_by_i16[3]; }

template<> inline int32 simd_get_one<0, int32, 8> (const SIMDType<8>& data) noexcept { return data.values_by_i32[0]; }
template<> inline int32 simd_get_one<1, int32, 8> (const SIMDType<8>& data) noexcept { return data.values_by_i32[1]; }

// set one component
template<> inline void simd_set_one<0, int8 , 8> (SIMDType<8>& data, int8 value) noexcept { data.values_by_i8[0] = value; }
template<> inline void simd_set_one<1, int8 , 8> (SIMDType<8>& data, int8 value) noexcept { data.values_by_i8[1] = value; }
template<> inline void simd_set_one<2, int8 , 8> (SIMDType<8>& data, int8 value) noexcept { data.values_by_i8[2] = value; }
template<> inline void simd_set_one<3, int8 , 8> (SIMDType<8>& data, int8 value) noexcept { data.values_by_i8[3] = value; }
template<> inline void simd_set_one<4, int8 , 8> (SIMDType<8>& data, int8 value) noexcept { data.values_by_i8[4] = value; }
template<> inline void simd_set_one<5, int8 , 8> (SIMDType<8>& data, int8 value) noexcept { data.values_by_i8[5] = value; }
template<> inline void simd_set_one<6, int8 , 8> (SIMDType<8>& data, int8 value) noexcept { data.values_by_i8[6] = value; }
template<> inline void simd_set_one<7, int8 , 8> (SIMDType<8>& data, int8 value) noexcept { data.values_by_i8[7] = value; }

template<> inline void simd_set_one<0, int16, 8> (SIMDType<8>& data, int16 value) noexcept { data.values_by_i16[0] = value; }
template<> inline void simd_set_one<1, int16, 8> (SIMDType<8>& data, int16 value) noexcept { data.values_by_i16[1] = value; }
template<> inline void simd_set_one<2, int16, 8> (SIMDType<8>& data, int16 value) noexcept { data.values_by_i16[2] = value; }
template<> inline void simd_set_one<3, int16, 8> (SIMDType<8>& data, int16 value) noexcept { data.values_by_i16[3] = value; }

template<> inline void simd_set_one<0, int32, 8> (SIMDType<8>& data, int32 value) noexcept { data.values_by_i32[1] = value; }
template<> inline void simd_set_one<1, int32, 8> (SIMDType<8>& data, int32 value) noexcept { data.values_by_i32[0] = value; }

// set all components by specifying all of them
template<> inline void simd_set_all<int8,  8> (SIMDType<8>& target, int8 a, int8 b, int8 c, int8 d, int8 e, int8 f, int8 g, int8 h) noexcept { target.simd_value = _mm_set_pi8(h, g, f, e, d, c, b, a); }
template<> inline void simd_set_all<int16, 8> (SIMDType<8>& target, int16 a, int16 b, int16 c, int16 d) noexcept { target.simd_value = _mm_set_pi16(d, c, b, a); }
template<> inline void simd_set_all<int32, 8> (SIMDType<8>& target, int32 a, int32 b) noexcept { target.simd_value = _mm_set_pi32(b, a); }
//template<> inline void simd_set_all<int64, 8> (SIMDType<8>& target, int64 value) noexcept { target.simd_value = _mm_cvtsi64_m64(value); }

// set all components using one same value
template<> inline void simd_broadcast<int8,  8> (SIMDType<8>& target, int8  value) noexcept { target.simd_value = _mm_set1_pi8 (value); }
template<> inline void simd_broadcast<int16, 8> (SIMDType<8>& target, int16 value) noexcept { target.simd_value = _mm_set1_pi16(value); }
template<> inline void simd_broadcast<int32, 8> (SIMDType<8>& target, int32 value) noexcept { target.simd_value = _mm_set1_pi32(value); }

// arithmetical and logical operations
template<> inline void simd_add<int8,  8> (SIMDType<8>& target, const SIMDType<8>& a, const SIMDType<8>& b) noexcept { target.simd_value = _mm_add_pi8 (a.simd_value, b.simd_value); }
template<> inline void simd_add<int16, 8> (SIMDType<8>& target, const SIMDType<8>& a, const SIMDType<8>& b) noexcept { target.simd_value = _mm_add_pi16(a.simd_value, b.simd_value); }
template<> inline void simd_add<int32, 8> (SIMDType<8>& target, const SIMDType<8>& a, const SIMDType<8>& b) noexcept { target.simd_value = _mm_add_pi32(a.simd_value, b.simd_value); }

template<> inline void simd_sub<int8,  8> (SIMDType<8>& target, const SIMDType<8>& a, const SIMDType<8>& b) noexcept { target.simd_value = _mm_sub_pi8 (a.simd_value, b.simd_value); }
template<> inline void simd_sub<int16, 8> (SIMDType<8>& target, const SIMDType<8>& a, const SIMDType<8>& b) noexcept { target.simd_value = _mm_sub_pi16(a.simd_value, b.simd_value); }
template<> inline void simd_sub<int32, 8> (SIMDType<8>& target, const SIMDType<8>& a, const SIMDType<8>& b) noexcept { target.simd_value = _mm_sub_pi32(a.simd_value, b.simd_value); }

template<> inline void simd_and<int, 8>  (SIMDType<8>& target, const SIMDType<8>& a, const SIMDType<8>& b) noexcept { target.simd_value = _mm_and_si64(a.simd_value, b.simd_value); }
template<> inline void simd_or<int, 8>   (SIMDType<8>& target, const SIMDType<8>& a, const SIMDType<8>& b) noexcept { target.simd_value = _mm_or_si64(a.simd_value, b.simd_value); }
template<> inline void simd_xor<int, 8>  (SIMDType<8>& target, const SIMDType<8>& a, const SIMDType<8>& b) noexcept { target.simd_value = _mm_xor_si64(a.simd_value, b.simd_value); }
template<> inline void simd_nand<int, 8> (SIMDType<8>& target, const SIMDType<8>& a, const SIMDType<8>& b) noexcept { target.simd_value = _mm_andnot_si64(a.simd_value, b.simd_value); }

template<> inline void simd_cmp<int8,  8>  (SIMDType<8>& target, const SIMDType<8>& a, const SIMDType<8>& b) noexcept { target.simd_value = _mm_cmpeq_pi8 (a.simd_value, b.simd_value); }
template<> inline void simd_cmp<int16, 8>  (SIMDType<8>& target, const SIMDType<8>& a, const SIMDType<8>& b) noexcept { target.simd_value = _mm_cmpeq_pi16(a.simd_value, b.simd_value); }
template<> inline void simd_cmp<int32, 8>  (SIMDType<8>& target, const SIMDType<8>& a, const SIMDType<8>& b) noexcept { target.simd_value = _mm_cmpeq_pi32(a.simd_value, b.simd_value); }

template<> inline void simd_gt<int8,  8>  (SIMDType<8>& target, const SIMDType<8>& a, const SIMDType<8>& b) noexcept { target.simd_value = _mm_cmpgt_pi8 (a.simd_value, b.simd_value); }
template<> inline void simd_gt<int16, 8>  (SIMDType<8>& target, const SIMDType<8>& a, const SIMDType<8>& b) noexcept { target.simd_value = _mm_cmpgt_pi16(a.simd_value, b.simd_value); }
template<> inline void simd_gt<int32, 8>  (SIMDType<8>& target, const SIMDType<8>& a, const SIMDType<8>& b) noexcept { target.simd_value = _mm_cmpgt_pi32(a.simd_value, b.simd_value); }

template<> inline void simd_lt<int8,  8>  (SIMDType<8>& target, const SIMDType<8>& a, const SIMDType<8>& b) noexcept { target.simd_value = _mm_cmpgt_pi8 (b.simd_value, a.simd_value); }
template<> inline void simd_lt<int16, 8>  (SIMDType<8>& target, const SIMDType<8>& a, const SIMDType<8>& b) noexcept { target.simd_value = _mm_cmpgt_pi16(b.simd_value, a.simd_value); }
template<> inline void simd_lt<int32, 8>  (SIMDType<8>& target, const SIMDType<8>& a, const SIMDType<8>& b) noexcept { target.simd_value = _mm_cmpgt_pi32(b.simd_value, a.simd_value); }

template<> inline bool simd_equal<int8, 8>(const SIMDType<8>& a, const SIMDType<8>& b) noexcept
{
    return a.value_by_i64 == b.value_by_i64;
}

template<> inline bool simd_equal<int16, 8>(const SIMDType<8>& a, const SIMDType<8>& b) noexcept
{
    return a.value_by_i64 == b.value_by_i64;
}

template<> inline bool simd_equal<int32, 8>(const SIMDType<8>& a, const SIMDType<8>& b) noexcept
{
    return a.value_by_i64 == b.value_by_i64;
}

#endif // 64bit MSVC

} // namespace treecore

#endif // TREECORE_SIMD_FUNC_SSE2_H
