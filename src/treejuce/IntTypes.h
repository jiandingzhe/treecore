#ifndef TREEJUCE_INT_TYPES_H
#define TREEJUCE_INT_TYPES_H

#include "treejuce/Common.h"

#include <cstdint>

namespace treejuce {

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

#ifdef TREEJUCE_COMPILER_MSVC
typedef pointer_sized_int ssize_t;
#endif

}

#endif // TREEJUCE_INT_TYPES_H
