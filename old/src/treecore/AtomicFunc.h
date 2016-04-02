#ifndef TREECORE_ATOMIC_FUNC_H
#define TREECORE_ATOMIC_FUNC_H

#ifdef __GNUC__
#  include "treecore/Atomic/gcc.h"
#elif defined _MSC_VER
#  include "treecore/Atomic/msvc.h"
#else
#  error "atomic support is not implemented"
#endif

#endif // TREECORE_ATOMIC_FUNC_H
