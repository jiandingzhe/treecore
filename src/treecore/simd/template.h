#ifndef TREECORE_SIMD_FUNC_TEMP_H
#define TREECORE_SIMD_FUNC_TEMP_H

#include "treecore/Common.h"
#include "treecore/PlatformDefs.h"

namespace treecore
{

template<int SZ>
union SIMDType;

template<int IDX, typename T, int SZ> inline T    simd_get_one( const SIMDType<SZ>& target ) noexcept;
template<int IDX, typename T, int SZ> inline void simd_set_one( SIMDType<SZ>& target, T value ) noexcept;

template<typename T, int SZ> inline void simd_get_all( const SIMDType<SZ>& target, T* values ) noexcept;

template<typename T, int SZ> inline void simd_set_all( SIMDType<SZ>& target,
                                                       T v0, T v1, T v2, T v3, T v4, T v5, T v6, T v7,
                                                       T v8, T v9, T v10, T v11, T v12, T v13, T v14, T v15 ) noexcept;
template<typename T, int SZ> inline void simd_set_all( SIMDType<SZ>& target, T a, T b, T c, T d, T e, T f, T g, T h ) noexcept;
template<typename T, int SZ> inline void simd_set_all( SIMDType<SZ>& target, T a, T b, T c, T d ) noexcept;
template<typename T, int SZ> inline void simd_set_all( SIMDType<SZ>& target, T a, T b ) noexcept;
template<typename T, int SZ> inline void simd_set_all( SIMDType<SZ>& target, T value ) noexcept;
template<typename T, int SZ> inline void simd_set_all( SIMDType<SZ>& target, const T* values ) noexcept;

template<typename T, int SZ> inline void simd_broadcast( SIMDType<SZ>& target, T value ) noexcept;

template<typename T, int SZ> inline void simd_add( SIMDType<SZ>& target, const SIMDType<SZ>& a, const SIMDType<SZ>& b ) noexcept;
template<typename T, int SZ> inline void simd_sub( SIMDType<SZ>& target, const SIMDType<SZ>& a, const SIMDType<SZ>& b ) noexcept;
template<typename T, int SZ> inline void simd_mul( SIMDType<SZ>& target, const SIMDType<SZ>& a, const SIMDType<SZ>& b ) noexcept;
template<typename T, int SZ> inline void simd_div( SIMDType<SZ>& target, const SIMDType<SZ>& a, const SIMDType<SZ>& b ) noexcept;

template<typename OutputType, typename InputType, int SZ> inline void simd_convert( SIMDType<SZ>& target, SIMDType<SZ>& input ) noexcept;

template<typename T, int SZ> inline void simd_cmpl( SIMDType<SZ>& target ) noexcept;
template<typename T, int SZ> inline void simd_and( SIMDType<SZ>& target, const SIMDType<SZ>& a, const SIMDType<SZ>& b ) noexcept;
template<typename T, int SZ> inline void simd_or( SIMDType<SZ>& target, const SIMDType<SZ>& a, const SIMDType<SZ>& b ) noexcept;
template<typename T, int SZ> inline void simd_xor( SIMDType<SZ>& target, const SIMDType<SZ>& a, const SIMDType<SZ>& b ) noexcept;
template<typename T, int SZ> inline void simd_nand( SIMDType<SZ>& target, const SIMDType<SZ>& a, const SIMDType<SZ>& b ) noexcept;

template<typename T, int SZ> inline void simd_shift_left( SIMDType<SZ>& result, const SIMDType<SZ>& input, int shift_value ) noexcept;
template<typename T, int SZ> inline void simd_shift_right_zero( SIMDType<SZ>& result, const SIMDType<SZ>& input, int shift_value ) noexcept;
template<typename T, int SZ> inline void simd_shift_right_sign( SIMDType<SZ>& result, const SIMDType<SZ>& input, int shift_value ) noexcept;

template<int SHIFT_COUNT, int SZ> inline void simd_shift_byte_left( SIMDType<SZ>& result, const SIMDType<SZ>& input ) noexcept;
template<int SHIFT_COUNT, int SZ> inline void simd_shift_byte_right( SIMDType<SZ>& result, const SIMDType<SZ>& input ) noexcept;

template<typename T, int SZ> inline void simd_cmp( SIMDType<SZ>& target, const SIMDType<SZ>& a, const SIMDType<SZ>& b ) noexcept;
template<typename T, int SZ> inline void simd_ncmp( SIMDType<SZ>& target, const SIMDType<SZ>& a, const SIMDType<SZ>& b ) noexcept;
template<typename T, int SZ> inline void simd_gt( SIMDType<SZ>& target, const SIMDType<SZ>& a, const SIMDType<SZ>& b ) noexcept;
template<typename T, int SZ> inline void simd_ge( SIMDType<SZ>& target, const SIMDType<SZ>& a, const SIMDType<SZ>& b ) noexcept;
template<typename T, int SZ> inline void simd_lt( SIMDType<SZ>& target, const SIMDType<SZ>& a, const SIMDType<SZ>& b ) noexcept;
template<typename T, int SZ> inline void simd_le( SIMDType<SZ>& target, const SIMDType<SZ>& a, const SIMDType<SZ>& b ) noexcept;

template<typename T, int SZ> inline bool simd_equal( const SIMDType<SZ>& a, const SIMDType<SZ>& b ) noexcept;

template<int IDX1, int IDX2, int IDX3, int IDX4, int SZ> inline void simd_shuffle( SIMDType<SZ>& target ) noexcept;

template<typename T, int SZ>
T simd_sum( const SIMDType<SZ>& value ) noexcept;

} // namespace treecore

#endif // TREECORE_SIMD_FUNC_TEMP_H
