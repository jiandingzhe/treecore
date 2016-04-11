#ifndef TREECORE_ATOMIC_FUNC_H
#define TREECORE_ATOMIC_FUNC_H

#include "treecore/PlatformDefs.h"

#if TREECORE_COMPILER_ATTR_MSVC
#  include "treecore/Atomic/msvc.h"
#elif TREECORE_COMPILER_ATTR_GCC || TREECORE_COMPILER_ATTR_CLANG
#  include "treecore/Atomic/gcc.h"
#else
#  error "atomic support is not implemented"
#endif

#endif // TREECORE_ATOMIC_FUNC_H
