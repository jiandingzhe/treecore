#ifndef TREECORE_INT_UTILS_H
#define TREECORE_INT_UTILS_H

#include "treecore/IntTypes.h"

namespace treecore
{

template<int SZ>
struct sized_int;

template<> struct sized_int<1> { typedef int8 type; };
template<> struct sized_int<2> { typedef int16 type; };
template<> struct sized_int<4> { typedef int32 type; };
template<> struct sized_int<8> { typedef int64 type; };

template<int SZ>
struct sized_uint;

template<> struct sized_uint<1> { typedef uint8 type; };
template<> struct sized_uint<2> { typedef uint16 type; };
template<> struct sized_uint<4> { typedef uint32 type; };
template<> struct sized_uint<8> { typedef uint64 type; };

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
