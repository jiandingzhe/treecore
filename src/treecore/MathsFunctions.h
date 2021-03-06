/*
   ==============================================================================

   This file is part of the juce_core module of the JUCE library.
   Copyright (c) 2013 - Raw Material Software Ltd.

   Permission to use, copy, modify, and/or distribute this software for any purpose with
   or without fee is hereby granted, provided that the above copyright notice and this
   permission notice appear in all copies.

   THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH REGARD
   TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS. IN
   NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL
   DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER
   IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN
   CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.

   ------------------------------------------------------------------------------

   NOTE! This permissive ISC license applies ONLY to files within the juce_core module!
   All other JUCE modules are covered by a dual GPL/commercial license, so if you are
   using any other modules, be sure to check that you also comply with their license.

   For more details, visit www.juce.com

   ==============================================================================
 */

#ifndef TREECORE_MATHS_FUNCTIONS_H
#define TREECORE_MATHS_FUNCTIONS_H

#include "treecore/DebugUtils.h"
#include "treecore/IntTypes.h"

#include <cmath>
#include <utility>

namespace treecore {

//==============================================================================
/*
    This file sets up some handy mathematical typdefs and functions.
 */

#if !TREECORE_COMPILER_DOXYGEN
/** A macro for creating 64-bit literals.
     Historically, this was needed to support portability with MSVC6, and is kept here
     so that old code will still compile, but nowadays every compiler will support the
     LL and ULL suffixes, so you should use those in preference to this macro.
 */
#    define literal64bit( longLiteral )     (longLiteral ## LL)
#endif

//==============================================================================
// Some indispensible min/max functions

/** Returns the larger of two values. */
template<typename Type>
inline Type jmax( const Type a, const Type b )                                               { return (a < b) ? b : a; }

/** Returns the larger of three values. */
template<typename Type>
inline Type jmax( const Type a, const Type b, const Type c )                                 { return (a < b) ? ( (b < c) ? c : b ) : ( (a < c) ? c : a ); }

/** Returns the larger of four values. */
template<typename Type>
inline Type jmax( const Type a, const Type b, const Type c, const Type d )                   { return jmax( a, jmax( b, c, d ) ); }

/** Returns the smaller of two values. */
template<typename Type>
inline Type jmin( const Type a, const Type b )                                               { return (b < a) ? b : a; }

/** Returns the smaller of three values. */
template<typename Type>
inline Type jmin( const Type a, const Type b, const Type c )                                 { return (b < a) ? ( (c < b) ? c : b ) : ( (c < a) ? c : a ); }

/** Returns the smaller of four values. */
template<typename Type>
inline Type jmin( const Type a, const Type b, const Type c, const Type d )                   { return jmin( a, jmin( b, c, d ) ); }

/** Scans an array of values, returning the minimum value that it contains. */
template<typename Type>
const Type findMinimum( const Type* data, int numValues )
{
    if (numValues <= 0)
        return Type();

    Type result( *data++ );

    while (--numValues > 0) // (> 0 rather than >= 0 because we've already taken the first sample)
    {
        const Type& v = *data++;
        if (v < result)  result = v;
    }

    return result;
}

/** Scans an array of values, returning the maximum value that it contains. */
template<typename Type>
const Type findMaximum( const Type* values, int numValues )
{
    if (numValues <= 0)
        return Type();

    Type result( *values++ );

    while (--numValues > 0) // (> 0 rather than >= 0 because we've already taken the first sample)
    {
        const Type& v = *values++;
        if (result < v)  result = v;
    }

    return result;
}

/** Scans an array of values, returning the minimum and maximum values that it contains. */
template<typename Type>
void findMinAndMax( const Type* values, int numValues, Type& lowest, Type& highest )
{
    if (numValues <= 0)
    {
        lowest  = Type();
        highest = Type();
    }
    else
    {
        Type mn( *values++ );
        Type mx( mn );

        while (--numValues > 0) // (> 0 rather than >= 0 because we've already taken the first sample)
        {
            const Type& v = *values++;

            if (mx < v)  mx = v;
            if (v < mn)  mn = v;
        }

        lowest  = mn;
        highest = mx;
    }
}

//==============================================================================
/** Constrains a value to keep it within a given range.

    This will check that the specified value lies between the lower and upper bounds
    specified, and if not, will return the nearest value that would be in-range. Effectively,
    it's like calling jmax (lowerLimit, jmin (upperLimit, value)).

    Note that it expects that lowerLimit <= upperLimit. If this isn't true,
    the results will be unpredictable.

    @param lowerLimit           the minimum value to return
    @param upperLimit           the maximum value to return
    @param valueToConstrain     the value to try to return
    @returns    the closest value to valueToConstrain which lies between lowerLimit
                and upperLimit (inclusive)
    @see jlimit0To, jmin, jmax
 */
template<typename Type>
inline Type jlimit( const Type lowerLimit,
                    const Type upperLimit,
                    const Type valueToConstrain ) noexcept
{
    treecore_assert( lowerLimit <= upperLimit ); // if these are in the wrong order, results are unpredictable..

    return (valueToConstrain < lowerLimit) ? lowerLimit
           : ( (upperLimit < valueToConstrain) ? upperLimit
               : valueToConstrain );
}

/** Returns true if a value is at least zero, and also below a specified upper limit.
    This is basically a quicker way to write:
    @code valueToTest >= 0 && valueToTest < upperLimit
    @endcode
 */
template<typename Type>
inline bool isPositiveAndBelow( Type valueToTest, Type upperLimit ) noexcept
{
    treecore_assert( Type() <= upperLimit ); // makes no sense to call this if the upper limit is itself below zero..
    return Type() <= valueToTest && valueToTest < upperLimit;
}

template<>
inline bool isPositiveAndBelow( const int valueToTest, const int upperLimit ) noexcept
{
    treecore_assert( upperLimit >= 0 ); // makes no sense to call this if the upper limit is itself below zero..
    return static_cast<unsigned int>(valueToTest) < static_cast<unsigned int>(upperLimit);
}

/** Returns true if a value is at least zero, and also less than or equal to a specified upper limit.
    This is basically a quicker way to write:
    @code valueToTest >= 0 && valueToTest <= upperLimit
    @endcode
 */
template<typename Type>
inline bool isPositiveAndNotGreaterThan( Type valueToTest, Type upperLimit ) noexcept
{
    treecore_assert( Type() <= upperLimit ); // makes no sense to call this if the upper limit is itself below zero..
    return Type() <= valueToTest && valueToTest <= upperLimit;
}

template<>
inline bool isPositiveAndNotGreaterThan( const int valueToTest, const int upperLimit ) noexcept
{
    treecore_assert( upperLimit >= 0 ); // makes no sense to call this if the upper limit is itself below zero..
    return static_cast<unsigned int>(valueToTest) <= static_cast<unsigned int>(upperLimit);
}

//==============================================================================
/** Handy function to swap two values. */
template<typename Type>
inline void swapVariables( Type& variable1, Type& variable2 )
{
    std::swap( variable1, variable2 );
}

/** Handy function for avoiding unused variables warning. */
template<typename Type1>
void ignoreUnused( const Type1& ) noexcept {}

template<typename Type1, typename Type2>
void ignoreUnused( const Type1&, const Type2& ) noexcept {}

template<typename Type1, typename Type2, typename Type3>
void ignoreUnused( const Type1&, const Type2&, const Type3& ) noexcept {}

template<typename Type1, typename Type2, typename Type3, typename Type4>
void ignoreUnused( const Type1&, const Type2&, const Type3&, const Type4& ) noexcept {}

/** Handy function for getting the number of elements in a simple const C array.
    E.g.
    @code
    static int myArray[] = { 1, 2, 3 };

    int numElements = numElementsInArray (myArray) // returns 3
    @endcode
 */
template<typename Type, int N>
inline int numElementsInArray( Type(&array)[N] )
{
    (void) array; // (required to avoid a spurious warning in MS compilers)
    (void) sizeof(0[array]);  // This line should cause an error if you pass an object with a user-defined subscript operator
    return N;
}

//==============================================================================
// Some useful maths functions that aren't always present with all compilers and build settings.

/** Using hypot is easier than dealing with the different types of hypot function
    that are provided by the various platforms and compilers. */
template<typename Type>
inline Type hypot( Type a, Type b ) noexcept
{
    return std::hypot( a, b );
}

/** 64-bit abs function. */
inline int64 abs64( const int64 n ) noexcept
{
    return (n >= 0) ? n : -n;
}

#if TREECORE_COMPILER_MSVC && !defined (DOXYGEN)   // The MSVC libraries omit these functions for some reason...
template<typename Type> Type asinh( Type x ) noexcept  { return std::log( x + std::sqrt( x * x + (Type) 1 ) ); }
template<typename Type> Type acosh( Type x ) noexcept  { return std::log( x + std::sqrt( x * x - (Type) 1 ) ); }
template<typename Type> Type atanh( Type x ) noexcept  { return ( std::log( x + (Type) 1 ) - std::log( ( (Type) 1 ) - x ) ) / (Type) 2; }
#endif

//==============================================================================
/** A predefined value for Pi, at double-precision.
    @see float_Pi
 */
const double double_Pi = 3.1415926535897932384626433832795;

/** A predefined value for Pi, at single-precision.
    @see double_Pi
 */
const float float_Pi = 3.14159265358979323846f;

//==============================================================================
/** The isfinite() method seems to vary between platforms, so this is a
    platform-independent function for it.
 */
template<typename FloatingPointType>
inline bool isfinite( FloatingPointType value )
{
#if TREECORE_OS_WINDOWS
    return _finite( value ) != 0;
#elif TREECORE_OS_ANDROID
    return isfinite( value );
#else
    return std::isfinite( value );
#endif
}

//==============================================================================
#if TREECORE_COMPILER_MSVC
#    pragma optimize ("t", off)
#    ifndef __INTEL_COMPILER
#        pragma float_control (precise, on, push)
#    endif
#endif

/** Fast floating-point-to-integer conversion.

    This is faster than using the normal c++ cast to convert a float to an int, and
    it will round the value to the nearest integer, rather than rounding it down
    like the normal cast does.

    Note that this routine gets its speed at the expense of some accuracy, and when
    rounding values whose floating point component is exactly 0.5, odd numbers and
    even numbers will be rounded up or down differently.
 */
template<typename FloatType>
inline int roundToInt( const FloatType value ) noexcept
{
#if TREECORE_COMPILER_ICC
#    pragma float_control (precise, on, push)
#endif

    union { int asInt[2]; double asDouble; } n;
    n.asDouble = ( (double) value ) + 6755399441055744.0;

#if TREECORE_ENDIAN_BIG
    return n.asInt [1];
#else
    return n.asInt [0];
#endif
}

inline int roundToInt( int value ) noexcept
{
    return value;
}

#if TREECORE_COMPILER_ATTR_MSVC
#    if !TREECORE_COMPILER_ICC
#        pragma float_control (pop)
#    endif
#    pragma optimize ("", on)// resets optimisations to the project defaults
#endif

/** Fast floating-point-to-integer conversion.

    This is a slightly slower and slightly more accurate version of roundDoubleToInt(). It works
    fine for values above zero, but negative numbers are rounded the wrong way.
 */
inline int roundToIntAccurate( const double value ) noexcept
{
#if TREECORE_COMPILER_ICC
#    pragma float_control (pop)
#endif

    return roundToInt( value + 1.5e-8 );
}

/** Fast floating-point-to-integer conversion.

    This is faster than using the normal c++ cast to convert a double to an int, and
    it will round the value to the nearest integer, rather than rounding it down
    like the normal cast does.

    Note that this routine gets its speed at the expense of some accuracy, and when
    rounding values whose floating point component is exactly 0.5, odd numbers and
    even numbers will be rounded up or down differently. For a more accurate conversion,
    see roundDoubleToIntAccurate().
 */
inline int roundDoubleToInt( const double value ) noexcept
{
    return roundToInt( value );
}

/** Fast floating-point-to-integer conversion.

    This is faster than using the normal c++ cast to convert a float to an int, and
    it will round the value to the nearest integer, rather than rounding it down
    like the normal cast does.

    Note that this routine gets its speed at the expense of some accuracy, and when
    rounding values whose floating point component is exactly 0.5, odd numbers and
    even numbers will be rounded up or down differently.
 */
inline int roundFloatToInt( const float value ) noexcept
{
    return roundToInt( value );
}

//==============================================================================
/** Returns true if the specified integer is a power-of-two.
 */
template<typename IntegerType>
bool isPowerOfTwo( IntegerType value )
{
    return ( value & (value - 1) ) == 0;
}

/** Returns the smallest power-of-two which is equal to or greater than the given integer.
 */
inline int nextPowerOfTwo( int n ) noexcept
{
    --n;
    n |= (n >> 1);
    n |= (n >> 2);
    n |= (n >> 4);
    n |= (n >> 8);
    n |= (n >> 16);
    return n + 1;
}

/** Returns the number of bits in a 32-bit integer. */
inline int countNumberOfBits( uint32 n ) noexcept
{
    n -= ( (n >> 1) & 0x55555555 );
    n  =  ( ( (n >> 2) & 0x33333333 ) + (n & 0x33333333) );
    n  =  ( ( (n >> 4) + n ) & 0x0f0f0f0f );
    n += (n >> 8);
    n += (n >> 16);
    return (int) (n & 0x3f);
}

/** Returns the number of bits in a 64-bit integer. */
inline int countNumberOfBits( uint64 n ) noexcept
{
    return countNumberOfBits( (uint32) n ) + countNumberOfBits( (uint32) (n >> 32) );
}

/** Performs a modulo operation, but can cope with the dividend being negative.
    The divisor must be greater than zero.
 */
template<typename IntegerType>
IntegerType negativeAwareModulo( IntegerType dividend, const IntegerType divisor ) noexcept
{
    treecore_assert( divisor > 0 );
    dividend %= divisor;
    return (dividend < 0) ? (dividend + divisor) : dividend;
}

/** Returns the square of its argument. */
template<typename NumericType>
NumericType square( NumericType n ) noexcept
{
    return n * n;
}

//==============================================================================
#if TREECORE_CPU_X86 || TREECORE_COMPILER_DOXYGEN
/** This macro can be applied to a float variable to check whether it contains a denormalised
     value, and to normalise it if necessary.
     On CPUs that aren't vulnerable to denormalisation problems, this will have no effect.
 */
#    define TREECORE_UNDENORMALISE( x )   x += 1.0f; x -= 1.0f;
#else
#    define TREECORE_UNDENORMALISE( x )
#endif

//==============================================================================
/** This namespace contains a few template classes for helping work out class type variations.
 */
namespace TypeHelpers
{

/** These templates are designed to take a type, and if it's a double, they return a double
        type; for anything else, they return a float type.
 */
template<typename Type> struct SmallestFloatType { typedef float type; };
template<>              struct SmallestFloatType<double>    { typedef double type; };
}

#define isPowOfTwo( number ) ( ( (number) > 1 ) && ( ( (number) & (number - 1) ) == 0 ) )
#define checkPowerOfTwo( number ) treecore_assert( ( (number) > 1 ) && ( ( (number) & (number - 1) ) == 0 ) )

//==============================================================================
}

#endif   // TREECORE_MATHS_FUNCTIONS_H
