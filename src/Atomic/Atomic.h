//!********************************************************************************
//! This file is part of the Three-Body Technology code base.
//! Copyright(C), 2012-2015, Three-Body Technology, all rights reserved. 
//! 
//! This file is protected by the copyright law of the people's Republic of China.
//! Permission to use this file is only granted for the employee of Three-Body 
//! Technology, or other company/group/individual who has expressly authorized by
//! Three-Body Technology. 
//!
//! @file    Atomic.h
//! @author  Meng Ke
//! @version 1.0
//! @date    2016/03/29
//! @brief   
//!********************************************************************************

#ifndef ____ATOMIC__4C755121_1D57_4F05_ABDE_197AC084AD08
#define ____ATOMIC__4C755121_1D57_4F05_ABDE_197AC084AD08


namespace treecore {


template <typename Type>
class Atomic
{
public:
    /** Creates a new value, initialised to zero. */
    forcedinline Atomic() noexcept
        //    : value ((Type)0)
    {
        helper::ResetToZero<volatile Type&>::Reset( value );
    }

    /** Creates a new value, with a given initial value. */
    forcedinline explicit Atomic( const Type& initialValue ) noexcept
        : value( initialValue )
    {}

    /** Copies another value (atomically). */
    forcedinline Atomic( const Atomic& other ) noexcept
        : value( other.get() )
    {}

    /** Destructor. */
    ~Atomic() = default;

    /** Atomically reads and returns the current value. */
    forcedinline Type get() const noexcept { return InterLocked::Load( &const_cast<Type&>( value ) ); }

    /** Copies another value onto this one (atomically). */
    forcedinline Atomic& operator= ( const Atomic& other ) noexcept { InterLocked::Store( &value , other.get() ); return *this; }

    /** Copies another value onto this one (atomically). */
    forcedinline Atomic& operator= ( const Type& newValue ) noexcept { InterLocked::Store( &value , newValue ); return *this; }

    /** Atomically sets the current value. */
    forcedinline void set( const Type& newValue ) noexcept { InterLocked::Store( &value , newValue ); }

    /** Atomically sets the current value, returning the value that was replaced. */
    forcedinline Type exchange( const Type& newValue ) noexcept { return InterLocked::Exchange( &value , newValue ); }

    /** Atomically adds a number to this value, returning the new value. */
    forcedinline Type operator+= ( Type amountToAdd ) noexcept { return InterLocked::AddExchange( &value , amountToAdd ); }

    /** Atomically subtracts a number from this value, returning the new value. */
    forcedinline Type operator-= ( Type amountToSubtract ) noexcept { return InterLocked::SubExchange( &value , amountToSubtract ); }

    //危险! 指针的++和--未能实现!!!!!

    /** Atomically increments this value, returning the new value. */
    forcedinline Type operator++() noexcept { return InterLocked::IncExchange( &value ); }

    /** Atomically decrements this value, returning the new value. */
    forcedinline Type operator--() noexcept { return InterLocked::DecExchange( &value ); }

    /** Atomically adds a number to this value, returning the new value. */
    forcedinline Type fetch_add( Type amountToAdd ) noexcept { return InterLocked::ExchangeAdd( &value , amountToAdd ); }

    /** Atomically subtracts a number from this value, returning the new value. */
    forcedinline Type fetch_sub( Type amountToSubtract ) noexcept { return InterLocked::ExchangeSub( &value , amountToSubtract ); }

    /** Atomically adds a number to this value, returning the new value. */
    forcedinline Type add_fetch( Type amountToAdd ) noexcept { return InterLocked::AddExchange( &value , amountToAdd ); }

    /** Atomically subtracts a number from this value, returning the new value. */
    forcedinline Type sub_fetch( Type amountToSubtract ) noexcept { return InterLocked::SubExchange( &value , amountToSubtract ); }

    /** Atomically increments this value, returning the new value. */
    forcedinline Type operator++( int ) noexcept { return InterLocked::ExchangeInc( &value ); }

    /** Atomically decrements this value, returning the new value. */
    forcedinline Type operator--( int ) noexcept { return InterLocked::ExchangeDec( &value ); }

    forcedinline bool compareAndSetBool( Type newValue , Type valueToCompare ) noexcept { return InterLocked::CompareSetBool( &value , valueToCompare , newValue ); }

    forcedinline Type compareAndSetValue( Type newValue , Type valueToCompare ) noexcept { return InterLocked::CompareSetValue( &value , valueToCompare , newValue ); }

    forcedinline bool compareAndSetValueAndBool( Type newValue , Type& valueToCompare ) noexcept { return InterLocked::CompareSetValueAndBool( &value , valueToCompare , newValue ); }

    forcedinline Type& get_raw() noexcept { return const_cast<Type&>( value ); }

    volatile Type value;
};


} // treecore


#endif // ____ATOMIC__4C755121_1D57_4F05_ABDE_197AC084AD08