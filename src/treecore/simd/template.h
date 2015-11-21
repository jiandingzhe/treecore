#ifndef TREECORE_SIMD_FUNC_TEMP_H
#define TREECORE_SIMD_FUNC_TEMP_H

#include "treecore/Common.h"

namespace treecore
{

template<int SZ>
union SIMDType;

template<int IDX, typename T, int SZ> inline T    simd_get_one(const SIMDType<SZ>& target);
template<int IDX, typename T, int SZ> inline void simd_set_one(SIMDType<SZ>& target, T value);

template<typename T, int SZ> inline void simd_get_all(const SIMDType<SZ>& target, T* values);

template<typename T, int SZ> inline void simd_set_all(SIMDType<SZ>& target, T a, T b, T c, T d, T e, T f, T g, T h);
template<typename T, int SZ> inline void simd_set_all(SIMDType<SZ>& target, T a, T b, T c, T d);
template<typename T, int SZ> inline void simd_set_all(SIMDType<SZ>& target, T a, T b);
template<typename T, int SZ> inline void simd_set_all(SIMDType<SZ>& target, T value);
template<typename T, int SZ> inline void simd_set_all(SIMDType<SZ>& target, const T* values);

template<typename T, int SZ> inline void simd_broadcast(SIMDType<SZ>& target, T value);

template<typename T, int SZ> inline void simd_add(SIMDType<SZ>& target, const SIMDType<SZ>&a, const SIMDType<SZ>&b);
template<typename T, int SZ> inline void simd_sub(SIMDType<SZ>& target, const SIMDType<SZ>&a, const SIMDType<SZ>&b);
template<typename T, int SZ> inline void simd_mul(SIMDType<SZ>& target, const SIMDType<SZ>&a, const SIMDType<SZ>&b);
template<typename T, int SZ> inline void simd_div(SIMDType<SZ>& target, const SIMDType<SZ>&a, const SIMDType<SZ>&b);

template<typename T, int SZ> inline void simd_and (SIMDType<SZ>& target, const SIMDType<SZ>&a, const SIMDType<SZ>&b);
template<typename T, int SZ> inline void simd_or  (SIMDType<SZ>& target, const SIMDType<SZ>&a, const SIMDType<SZ>&b);
template<typename T, int SZ> inline void simd_xor (SIMDType<SZ>& target, const SIMDType<SZ>&a, const SIMDType<SZ>&b);
template<typename T, int SZ> inline void simd_nand(SIMDType<SZ>& target, const SIMDType<SZ>&a, const SIMDType<SZ>&b);

template<typename T, int SZ> inline void simd_cmp (SIMDType<SZ>& target, const SIMDType<SZ>&a, const SIMDType<SZ>&b);
template<typename T, int SZ> inline void simd_gt  (SIMDType<SZ>& target, const SIMDType<SZ>&a, const SIMDType<SZ>&b);
template<typename T, int SZ> inline void simd_ge  (SIMDType<SZ>& target, const SIMDType<SZ>&a, const SIMDType<SZ>&b);
template<typename T, int SZ> inline void simd_lt  (SIMDType<SZ>& target, const SIMDType<SZ>&a, const SIMDType<SZ>&b);
template<typename T, int SZ> inline void simd_le  (SIMDType<SZ>& target, const SIMDType<SZ>&a, const SIMDType<SZ>&b);

template<int IDX1, int IDX2, int IDX3, int IDX4, int SZ> inline void simd_shuffle(SIMDType<SZ>& target);

template<typename T, int SZ>
T simd_sum(const SIMDType<SZ>&value);

} // namespace treecore

#endif // TREECORE_SIMD_FUNC_TEMP_H
