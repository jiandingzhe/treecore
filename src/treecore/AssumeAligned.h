#ifndef TREECORE_ALIGN_H
#define TREECORE_ALIGN_H

#include "treecore/Config.h"

/*

to help compiler build SIMD code, we better tell compiler that a pointer is aligned,
MSVC, INTEL ICC, GCC and Clang all have buildin "assume aligned" instence. 

*/

#define RCAST(ptr) reinterpret_cast<size_t>(ptr)

#if defined TREECORE_COMPILER_MSVC
#   define TREECORE_ASSUME_ALIGNED(ptr, alignment) __assume( (RCAST(ptr) & ((alignment) - 1)) == 0 ); __assume( RCAST(ptr) % (alignment) ) == 0 )
#elif defined TREECORE_COMPILER_ICC
#   define TREECORE_ASSUME_ALIGNED(ptr, alignment) __assume_aligned((ptr), (alignment))
#elif defined TREECORE_COMPILER_GCC || defined TREECORE_COMPILER_CLANG
#   define TREECORE_ASSUME_ALIGNED(ptr,alignment) (ptr)= __builtin_assume_aligned((ptr), (alignment))
#else
#   define TREECORE_ASSUME_ALIGNED(ptr,alignment)
#endif

#undef RCAST

#define TREECORE_ASSUME_ALIGNED16(ptr) TREECORE_ASSUME_ALIGNED(ptr,16)

#endif // TREECORE_ALIGN_H