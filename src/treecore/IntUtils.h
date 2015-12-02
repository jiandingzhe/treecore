#ifndef TREECORE_INT_UTILS_H
#define TREECORE_INT_UTILS_H

#include "treecore/IntTypes.h"

namespace treecore
{

template<int SZ>
struct sized_int;

template<> struct sized_int<1> { typedef int8  type; };
template<> struct sized_int<2> { typedef int16 type; };
template<> struct sized_int<4> { typedef int32 type; };
template<> struct sized_int<8> { typedef int64 type; };

template<int SZ>
struct sized_uint;

template<> struct sized_uint<1> { typedef uint8  type; };
template<> struct sized_uint<2> { typedef uint16 type; };
template<> struct sized_uint<4> { typedef uint32 type; };
template<> struct sized_uint<8> { typedef uint64 type; };


template<int SZ>
struct sized_ones_mask;

template<> struct sized_ones_mask<1> { static const uint8  value = 0xff; };
template<> struct sized_ones_mask<2> { static const uint16 value = 0xffff; };
template<> struct sized_ones_mask<4> { static const uint32 value = 0xffffffff; };
template<> struct sized_ones_mask<8> { static const uint64 value = 0xffffffffffffffff; };

template<typename T>
struct similar_int
{
    typedef typename sized_int<sizeof(T)>::type type;
};

template<typename T>
struct similar_uint
{
    typedef typename sized_uint<sizeof(T)>::type type;
};

} // namespace treecore

#endif // TREECORE_INT_UTILS_H
