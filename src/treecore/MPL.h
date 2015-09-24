#ifndef TREECORE_MPL_H
#define TREECORE_MPL_H

namespace treecore
{

template<bool, typename TrueT, typename FalseT>
struct mpl_type_if;

template<typename TrueT, typename FalseT>
struct mpl_type_if<true, TrueT, FalseT>
{
    typedef TrueT type;
};

template<typename TrueT, typename FalseT>
struct mpl_type_if<false, TrueT, FalseT>
{
    typedef FalseT type;
};

} // namespace treecore

#endif // TREECORE_MPL_H
