#ifndef TREEJUCE_HASH_FUNCTIONS_H
#define TREEJUCE_HASH_FUNCTIONS_H

#include "treejuce/MathsFunctions.h"
#include "treejuce/String.h"
#include "treejuce/Variant.h"

TREEFACE_JUCE_NAMESPACE_BEGIN

//==============================================================================

/**
    A simple class to generate hash functions for some primitive types, intended for
    use with the HashMap class.
    @see HashMap
*/
struct DefaultHashFunctions
{
    /** Generates a simple hash from an integer. */
    int generateHash (const int key, const int upperLimit) const NOEXCEPT        { return std::abs (key) % upperLimit; }
    /** Generates a simple hash from an int64. */
    int generateHash (const int64 key, const int upperLimit) const NOEXCEPT      { return std::abs ((int) key) % upperLimit; }
    /** Generates a simple hash from a string. */
    int generateHash (const String& key, const int upperLimit) const NOEXCEPT    { return (int) (((uint32) key.hashCode()) % (uint32) upperLimit); }
    /** Generates a simple hash from a variant. */
    int generateHash (const var& key, const int upperLimit) const NOEXCEPT       { return generateHash (key.toString(), upperLimit); }
};

TREEFACE_JUCE_NAMESPACE_END

#endif // TREEJUCE_HASH_FUNCTIONS_H
