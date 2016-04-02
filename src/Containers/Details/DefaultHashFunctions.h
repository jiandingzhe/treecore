#ifndef TREECORE_HASH_FUNCTIONS_H
#define TREECORE_HASH_FUNCTIONS_H

#include "treecore/Identifier.h"
#include "treecore/MathsFunctions.h"
#include "treecore/String.h"
#include "treecore/Variant.h"


namespace treecore {

//==============================================================================

/**
    A simple class to generate hash functions for some primitive types, intended for
    use with the HashMap class.
    @see HashMap
*/
struct DefaultHashFunctions
{
    /** Generates a simple hash from an integer. */
    int generateHash (const int key, const int upperLimit) const noexcept
    {
        return std::abs (key) % upperLimit;
    }

    /** Generates a simple hash from an int64. */
    int generateHash (const int64 key, const int upperLimit) const noexcept
    {
        return std::abs ((int) key) % upperLimit;
    }

    /** Generates a simple hash from a string. */
    int generateHash (const String& key, const int upperLimit) const noexcept
    {
        return (int) (((uint32) key.hashCode()) % (uint32) upperLimit);
    }

    /**
     * @brief hash a variant as string
     */
    int generateHash (const var& key, const int upperLimit) const noexcept
    {
        return generateHash (key.toString(), upperLimit);
    }

    /**
     * @brief hash an Identifier as integer value
     */
    int generateHash(const Identifier& key, const int upperLimit) const noexcept
    {
        return pointer_sized_uint(key.getPtr()) % upperLimit;
    }
};

}

#endif // TREECORE_HASH_FUNCTIONS_H
