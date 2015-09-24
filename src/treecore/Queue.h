#ifndef TREECORE_QUEUE_H
#define TREECORE_QUEUE_H

#include "treecore/QueueBase.h"

namespace treecore
{

template<typename T,typename IntType=size_t>
class Queue : public impl::QueueBase<T,IntType>
{
public:
    using impl::QueueBase<T,IntType>::m_p2size;
    explicit forcedinline Queue( const IntType p2size = 12 ) noexcept
        : impl::QueueBase<T,IntType>( p2size )
        , m_readStart( 0 )
        , m_writeStart( 0 )
    {
        jassert( p2size > 0 ); //p2size至少要为1,也就是说cirbuffer的尺寸最小为2(但只能容纳1个元素)
    }

    forcedinline ~Queue(){}

    template<typename Func> forcedinline bool bound_push( const Func& func )
    {
        const bool k = !isFull();
        likely_if( k ) func( ( *this )[m_writeStart++] );
        return k;
    }
    forcedinline bool bound_push( const T& obj )
    {
        return bound_push( [&]( T& k ){ k = obj; } );
    }

    template<typename Func> forcedinline void push( const Func& func )
    {
        while( unlikely( !bound_push( func ) ) ) {
            m_writeStart = this->realloc( ++m_p2size , m_readStart , m_writeStart );
            m_readStart = 0;
        }
    }

    forcedinline void push( const T& obj )
    {
        return push( [&]( T& k ){ k = obj; } );
    }

    template<typename Func> forcedinline bool pop( const Func& func )
    {
        const bool k = !isEmpty();
        likely_if( k ) func( ( *this )[m_readStart++] );
        return k;
    }
    forcedinline bool pop( T& obj )
    {
        return pop( [&]( T& k ){ obj = k; } );
    }
    forcedinline bool isEmpty() const { return this->mod( m_readStart ) == this->mod( m_writeStart ); }
    forcedinline bool isFull() const { return this->mod( m_readStart ) == this->mod( m_writeStart + 1 ); }
    forcedinline IntType getUsedSize() const
    {
        return getUsedSize( m_writeStart , m_readStart );
    }
private:
    IntType m_readStart;
    IntType m_writeStart;
    TREECORE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR( Queue );
};


} // namespace treecore

#endif // TREECORE_QUEUE_H
