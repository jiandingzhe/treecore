#ifndef TREECORE_INT_TYPES_H
#define TREECORE_INT_TYPES_H

#include "treecore/PlatformDefs.h"

#include <cstdint>
#include <cstdlib>
#include <unistd.h>

namespace treecore {

using std::size_t;

typedef std::int8_t int8;
typedef std::uint8_t uint8;

typedef std::int16_t int16;
typedef std::uint16_t uint16;

typedef std::int32_t int32;
typedef std::uint32_t uint32;

typedef std::int64_t int64;
typedef std::uint64_t uint64;

typedef std::intptr_t pointer_sized_int;
typedef std::uintptr_t pointer_sized_uint;

#if TREECORE_COMPILER_MSVC
typedef std::intptr_t ssize_t;
static_assert( sizeof(size_t) == sizeof(ssize_t), "size type validation" )
#else
using ::ssize_t;
#endif

} // namespace treecore

#endif // TREECORE_INT_TYPES_H
