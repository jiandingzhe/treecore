#ifndef TREECORE_ALIGN_H
#define TREECORE_ALIGN_H

#include "treecore/Config.h"

#if defined TREECORE_COMPILER_MSVC || (defined TREECORE_COMPILER_ICC && defined TREECORE_OS_WINDOWS)
#   define TREECORE_ALIGNOF(_type_) __builtin_alignof(_type_) // fuck MSVC which don't allow redefine keyword and don't provide that keyword
#   define TREECORE_ALN_BEGIN(x) __declspec(align(x))
#   define TREECORE_ALN_END(x)
#else
#   define TREECORE_ALIGNOF(_type_) __alignof__(_type_)
#   define TREECORE_ALN_BEGIN(x)
#   define TREECORE_ALN_END(x) __attribute__((aligned(x)))
#endif

#endif // TREECORE_ALIGN_H
