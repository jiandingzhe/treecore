#ifndef TREECORE_SIMILAR_FLOAT_H
#define TREECORE_SIMILAR_FLOAT_H

namespace treecore
{

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
