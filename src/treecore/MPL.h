#ifndef TREECORE_MPL_H
#define TREECORE_MPL_H

#include <cstddef>
#include <new>

#include "treecore/ClassUtil.h"


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

template <size_t T,size_t... Ts>
struct mpl_value_queue {
    TREECORE_FUNCTION_CLASS(mpl_value_queue);

    enum :size_t
    {
        head = T ,
        number = 1+sizeof...(Ts)
    };

    typedef mpl_value_queue<Ts...> pop_head;

public:
    template<size_t pushT>
    struct push_tail
    {
        typedef mpl_value_queue<T, Ts..., pushT> type;
    };

    template<size_t pushT>
    struct push_head
    {
        typedef mpl_value_queue<pushT, T, Ts...> type;
    };
};

template <size_t T>
struct mpl_value_queue<T> {
    TREECORE_FUNCTION_CLASS(mpl_value_queue);

    enum {
        head = T,
        number = 1
    };

    typedef void pop_head;

public:
    template<size_t pushT>
    struct push_tail
    {
        typedef mpl_value_queue<T, pushT> type;
    };

    template<size_t pushT>
    struct push_head
    {
        typedef mpl_value_queue<pushT, T> type;
    };
};

} // namespace treecore

#endif // TREECORE_MPL_H
