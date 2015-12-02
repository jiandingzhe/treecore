#ifndef TREECORE_SIMILAR_FLOAT_H
#define TREECORE_SIMILAR_FLOAT_H

#include "treecore/IntTypes.h"

namespace treecore
{

template<typename T>
struct float_sign_mask;

template<> struct float_sign_mask<float> { static const uint32 value = 0x80000000; };
template<> struct float_sign_mask<double> { static const uint64 value = 0x8000000000000000; };

namespace impl
{

template<bool large> struct SimilarFloatImpl;
template<> struct SimilarFloatImpl<false> { typedef float type; };
template<> struct SimilarFloatImpl<true>  { typedef double type; };
}

template<typename T>
struct similar_float
{
    typedef typename impl::SimilarFloatImpl< sizeof(T) >= sizeof(double) >::type type;
};

} // namespace treecore

#endif // TREECORE_SIMILAR_FLOAT_H
