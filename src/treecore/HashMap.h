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

#ifndef JUCE_HASHMAP_H_INCLUDED
#define JUCE_HASHMAP_H_INCLUDED

#include "treecore/impl/HashImpl.h"
#include "treecore/DummyCriticalSection.h"
#include "treecore/LeakedObjectDetector.h"

#include <unordered_map>

#define LOCK_HASH_MAP const ScopedLockType _lock_( m_mutex )

class TestFramework;

namespace treecore {

/**
    Holds a set of mappings between some key/value pairs.

    The types of the key and value objects are set as template parameters.
    You can also specify a class to supply a hash function that converts a key value
    into an hashed integer. This class must have the form:

    @code
    struct MyHashGenerator
    {
        int generateHash (MyKeyType key, int upperLimit) const
        {
            // The function must return a value 0 <= x < upperLimit
            return someFunctionOfMyKeyType (key) % upperLimit;
        }
    };
    @endcode

    Like the Array class, the key and value types are expected to be copy-by-value
    types, so if you define them to be pointer types, this class won't delete the
    objects that they point to.

    If you don't supply a class for the HashFunctionType template parameter, the
    default one provides some simple mappings for strings and ints.

    @code
    HashMap<int, String> hash;
    hash.set (1, "item1");
    hash.set (2, "item2");

    DBG (hash [1]); // prints "item1"
    DBG (hash [2]); // prints "item2"

    // This iterates the map, printing all of its key -> value pairs..
    for (HashMap<int, String>::Iterator i (hash); i.next();)
        DBG (i.getKey() << " -> " << i.getValue());
    @endcode

    @tparam HashFunctionType The class of hash function, which must be copy-constructible.
    @see CriticalSection, DefaultHashFunctions, NamedValueSet, SortedSet
 */
template<typename KeyType,
         typename ValueType,
         class HashFunctionType = DefaultHashFunctions,
         class MutexType = DummyCriticalSection>
class HashMap: public RefCountObject
{
    struct HashMapItem
    {
        HashMapItem() {}

        HashMapItem( const KeyType& key, const ValueType& value )
            : key( key )
            , value( value )
        {}

        HashMapItem( const KeyType& key, ValueType&& value )
            : key( key )
            , value( std::move( value ) )
        {}

        HashMapItem( HashMapItem&& peer )
            : key( peer.key )
            , value( std::move( peer.value ) )
        {}

        HashMapItem& operator = ( HashMapItem&& peer )
        {
            key   = std::move( peer.key );
            value = std::move( peer.value );
        }

        const KeyType key{};
        ValueType value{};

        bool operator == ( const HashMapItem& other ) const
        {
            return key == other.key && value == other.value;
        }

        bool operator != ( const HashMapItem& other ) const
        {
            return key != other.key || value != other.value;
        }
    };

    typedef impl::HashTableBase<KeyType, HashMapItem, HashFunctionType, CriticalSectionIsDummy<MutexType>::value> TableImplType;
    typedef typename TableImplType::HashEntry EntryType;

    friend class ::TestFramework;

public:
    //==============================================================================
    /** Iterates over the items in a HashMap.

        To use it, repeatedly call next() until it returns false, e.g.
        @code
        HashMap <String, String> myMap;

        HashMap<String, String>::Iterator i (myMap);

        while (i.next())
        {
            DBG (i.getKey() << " -> " << i.getValue());
        }
        @endcode

        The order in which items are iterated bears no resemblence to the order in which
        they were originally added!

        Obviously as soon as you call any non-const methods on the original hash-map, any
        iterators that were created beforehand will cease to be valid, and should not be used.

        @see HashMap
     */
    class Iterator
    {
        friend class HashMap;
        typedef typename TableImplType::template IteratorBase<TableImplType&, EntryType*> ItImplType;

public:
        //==============================================================================
        Iterator ( HashMap& target )
            : m_impl( target.m_impl )
        {}

        /** Moves to the next item, if one is available.
            When this returns true, you can get the item's key and value using getKey() and
            getValue(). If it returns false, the iteration has finished and you should stop.
         */
        bool next() noexcept
        {
            return m_impl.next();
        }

        /**
         * @brief whether the iterator has accessable content
         * @return true if the content of this iterator can be accessed
         */
        bool hasContent() const noexcept
        {
            return m_impl.entry != nullptr;
        }

        /**
         * @brief get current item's key.
         *
         * This should only be called when a call to next() has just returned true.
         */
        const KeyType& key() const
        {
            jassert( m_impl.entry != nullptr );
            return m_impl.entry->item.key;
        }

        /**
         * @brief get current item's value.
         *
         * This should only be called when a call to next() has just returned true.
         */
        ValueType& value()
        {
            jassert( m_impl.entry != nullptr );
            return m_impl.entry->item.value;
        }

private:
        ItImplType m_impl;

        TREECORE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR( Iterator )
    };

    class ConstIterator
    {
        friend class HashMap;
        typedef typename TableImplType::template IteratorBase<const TableImplType&, EntryType*> ItImplType;

public:
        //==============================================================================
        ConstIterator( const HashMap& target )
            : m_impl( target.m_impl )
        {}

        /** Moves to the next item, if one is available.
            When this returns true, you can get the item's key and value using getKey() and
            getValue(). If it returns false, the iteration has finished and you should stop.
         */
        bool next() noexcept
        {
            return m_impl.next();
        }

        /**
         * @brief whether the iterator has accessable content
         * @return true if the content of this iterator can be accessed
         */
        bool hasContent() const noexcept
        {
            return m_impl.entry != nullptr;
        }

        /**
         * @brief get current item's key.
         *
         * This should only be called when a call to next() has just returned true.
         */
        const KeyType& key() const noexcept
        {
            return m_impl.entry->item.key;
        }

        /**
         * @brief get current item's value.
         *
         * This should only be called when a call to next() has just returned true.
         */
        const ValueType& value() const noexcept
        {
            return m_impl.entry->item.value;
        }

private:
        ItImplType m_impl;

        TREECORE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR( ConstIterator )
    };

    /** Returns the type of scoped lock to use for locking this array */
    typedef typename MutexType::ScopedLockType ScopedLockType;

public:
    //==============================================================================
    /** Creates an empty hash-map.

        @param numberOfSlots Specifies the number of hash entries the map will use. This will be
                            the "upperLimit" parameter that is passed to your generateHash()
                            function. The number of hash slots will grow automatically if necessary,
                            or it can be remapped manually using remapTable().
        @param hashFunction An instance of HashFunctionType, which will be copied and
                            stored to use with the HashMap. This parameter can be omitted
                            if HashFunctionType has a default constructor.
     */
    explicit HashMap ( int numberOfSlots = defaultHashTableSize,
                       HashFunctionType hashFunction = HashFunctionType() )
        : m_impl( numberOfSlots, hashFunction )
    {}

    /**
     * @brief create HashMap by copying another table's contents
     */
    HashMap( const HashMap& other )
        : m_impl( other.m_impl )
        , m_mutex()
    {}

    HashMap( HashMap&& other )
        : m_impl( std::move( other.m_impl ) )
    {}

    /**
     * @brief destructor
     * @see HashTableBase::~HashTableBase()
     */
    virtual ~HashMap()
    {}

    HashMap& operator = ( const HashMap& other )
    {
        LOCK_HASH_MAP;
        const ScopedLockType sl_other( other.m_mutex );

        m_impl.clone_slots_from( other.m_impl );
        return *this;
    }

    bool operator == ( const HashMap& other ) const noexcept
    {
        LOCK_HASH_MAP;
        const ScopedLockType sl_other( other.m_mutex );
        return m_impl == other.m_impl;
    }

    bool operator != ( const HashMap& other ) const noexcept
    {
        LOCK_HASH_MAP;
        const ScopedLockType sl_other( other.m_mutex );
        return !(m_impl == other.m_impl);
    }

    //==============================================================================
    /**
     * @brief removes all values from the map
     *
     * Note that this will clear the content, but won't affect the number of slots (see
     * remapTable and getNumSlots).
     */
    void clear()
    {
        LOCK_HASH_MAP;
        m_impl.clear();
    }

    //==============================================================================
    /** Returns the current number of items in the map. */
    inline int size() const noexcept
    {
        return m_impl.num_entries;
    }

    /**
     * @brief returns the element corresponding to a given key
     *
     * If the map doesn't contain the key, a new item will be created.
     *
     * @param keyToLookFor the key of the item being requested
     *
     * @return reference to value stored by this key
     */
    inline ValueType& operator [] ( const KeyType& key ) noexcept
    {
        LOCK_HASH_MAP;
        int i_bucket = m_impl.bucket_index( key );

        // search existing entry
        EntryType* entry = m_impl.search_entry_at( i_bucket, key );

        // create new entry
        if (!entry)
        {
            entry = m_impl.create_entry_at( i_bucket, HashMapItem( key, ValueType{} ) );

            if ( m_impl.high_fill_rate() )
                m_impl.expand_buckets();
        }

        return entry->item.value;
    }

    /**
     * Returns true if the map contains an item with the specied key.
     */
    bool contains( const KeyType& key ) const noexcept
    {
        LOCK_HASH_MAP;
        int i_bucket = m_impl.bucket_index( key );
        EntryType* entry = m_impl.search_entry_at( i_bucket, key );
        return entry != nullptr;
    }

    /**
     * @brief get all keys in this table
     * @return an array containing all keys
     */
    Array<KeyType> getAllKeys() const
    {
        Array<KeyType> re;
        m_impl.get_all_keys( re );
        return re;
    }

    /**
     * @brief get item by key
     *
     * @param key     key to search for
     * @param result  This iterator will be modified to point to search result.
     *                If not found, it will not be modified.
     *
     * @return true if got key, false if not
     */
    bool select( const KeyType& key, Iterator& result ) noexcept
    {
        LOCK_HASH_MAP;
        int i_bucket = m_impl.bucket_index( key );
        EntryType* entry = m_impl.search_entry_at( i_bucket, key );

        if (entry)
        {
            result.m_impl.i_bucket = i_bucket;
            result.m_impl.entry    = entry;
        }

        return entry != nullptr;
    }

    bool select( const KeyType& key, ConstIterator& result ) const noexcept
    {
        LOCK_HASH_MAP;
        int i_bucket = m_impl.bucket_index( key );
        EntryType* entry = m_impl.search_entry_at( i_bucket, key );

        if (entry)
        {
            result.m_impl.i_bucket = i_bucket;
            result.m_impl.entry    = entry;
        }

        return entry != nullptr;
    }

    /**
     * @brief Store value if the key does not exist in table, and make iterator
     *        point to it.
     *
     * @param key     key to be stored
     * @param value   value to be stored
     * @param result  Iterator will be set to point to the newly inserted item,
     *                or the existing item that has the same key.
     *
     * @return True if insertion is actually performed, false if key already
     *         exists in table.
     */
    bool insertOrSelect( const KeyType& key, const ValueType& value, Iterator& result ) noexcept
    {
        LOCK_HASH_MAP;

        // search for existing entry
        int i_bucket = m_impl.bucket_index( key );
        EntryType* entry = m_impl.search_entry_at( i_bucket, key );

        if (entry != nullptr)
        {
            result.m_impl.i_bucket = i_bucket;
            result.m_impl.entry    = entry;
            return false;
        }

        // create new one
        entry = m_impl.create_entry_at( i_bucket, HashMapItem( key, value ) );

        if ( m_impl.high_fill_rate() )
        {
            m_impl.expand_buckets();
            i_bucket = m_impl.bucket_index( key );
        }

        result.m_impl.i_bucket = i_bucket;
        result.m_impl.entry    = entry;
        return true;
    }

    bool insertOrSelect( const KeyType& key, ValueType&& value, Iterator& result ) noexcept
    {
        LOCK_HASH_MAP;

        // search for existing entry
        int i_bucket = m_impl.bucket_index( key );
        EntryType* entry = m_impl.search_entry_at( i_bucket, key );

        if (entry != nullptr)
        {
            result.m_impl.i_bucket = i_bucket;
            result.m_impl.entry    = entry;
            return false;
        }

        // create new one
        entry = m_impl.create_entry_at( i_bucket, HashMapItem( key, std::move( value ) ) );

        if ( m_impl.high_fill_rate() )
        {
            m_impl.expand_buckets();
            i_bucket = m_impl.bucket_index( key );
        }

        result.m_impl.i_bucket = i_bucket;
        result.m_impl.entry    = entry;
        return true;
    }

    ///
    /// \brief store value if key is not exist
    /// \param key
    /// \param value
    /// \return true if insert success, false if key already exists
    ///
    bool tryInsert( const KeyType& key, const ValueType& value )
    {
        LOCK_HASH_MAP;

        // search for existing entry
        int i_bucket = m_impl.bucket_index( key );
        EntryType* entry = m_impl.search_entry_at( i_bucket, key );

        if (entry != nullptr) return false;

        // create new one
        entry = m_impl.create_entry_at( i_bucket, HashMapItem( key, value ) );

        if ( m_impl.high_fill_rate() )
        {
            m_impl.expand_buckets();
            i_bucket = m_impl.bucket_index( key );
        }

        return true;
    }

    bool tryInsert( const KeyType& key, ValueType&& value )
    {
        LOCK_HASH_MAP;

        // search for existing entry
        int i_bucket = m_impl.bucket_index( key );
        EntryType* entry = m_impl.search_entry_at( i_bucket, key );

        if (entry != nullptr) return false;

        // create new one
        entry = m_impl.create_entry_at( i_bucket, HashMapItem( key, std::move( value ) ) );

        if ( m_impl.high_fill_rate() )
        {
            m_impl.expand_buckets();
            i_bucket = m_impl.bucket_index( key );
        }

        return true;
    }

    /**
     * @brief returns true if the hash contains at least one occurrence of a
     *        given value.
     */
    bool containsValue( const ValueType& value ) const
    {
        LOCK_HASH_MAP;

        for (int i = 0; i < m_impl.buckets.size(); i++)
        {
            for (const EntryType* entry = m_impl.buckets[i]; entry != nullptr; entry = entry->next_entry)
            {
                if (entry->item.value == value)
                    return true;
            }
        }

        return false;
    }

    ValueType& getOrDefault( const KeyType& key, ValueType& defaultValue ) noexcept
    {
        return const_cast<ValueType&>( ( (const HashMap*) this )->getOrDefault( key, defaultValue ) );
    }

    const ValueType& getOrDefault( const KeyType& key, const ValueType& defaultValue ) const noexcept
    {
        LOCK_HASH_MAP;
        int i_bucket = m_impl.bucket_index( key );
        const EntryType* entry = m_impl.search_entry_at( i_bucket, key );

        if (entry)
            return entry->item.value;
        else
            return defaultValue;
    }

    /**
     * @brief adds or replaces an element in the hash-map
     *
     * If there's already an item with the given key, this will replace its
     * value. Otherwise, a new item will be added to the map.
     */
    void set( const KeyType& key, const ValueType& value ) noexcept
    {
        LOCK_HASH_MAP;
        int i_bucket = m_impl.bucket_index( key );

        // try to get existing entry
        EntryType* entry = m_impl.search_entry_at( i_bucket, key );

        if (entry)
        {
            entry->item.value = value;
        }
        else
        {
            // create new entry
            entry = m_impl.create_entry_at( i_bucket, HashMapItem{key, value} );

            if ( m_impl.high_fill_rate() )
                m_impl.expand_buckets();
        }
    }

    void set( const KeyType& key, ValueType&& value ) noexcept
    {
        LOCK_HASH_MAP;
        int i_bucket = m_impl.bucket_index( key );

        // try to get existing entry
        EntryType* entry = m_impl.search_entry_at( i_bucket, key );

        if (entry)
        {
            entry->item.value = std::move( value );
        }
        else
        {
            // create new entry
            entry = m_impl.create_entry_at( i_bucket, HashMapItem{key, std::move( value )} );

            if ( m_impl.high_fill_rate() )
                m_impl.expand_buckets();
        }
    }

    /**
     * @brief removes an item with the given key
     * @param key key to remove
     * @return true if removed, false if no this key
     */
    bool remove( const KeyType& key ) noexcept
    {
        LOCK_HASH_MAP;
        const int i_bucket = m_impl.bucket_index( key );
        return m_impl.remove_entry_at( i_bucket, key );
    }

    /**
     * @brief removes all items with the given value
     * @return number of items removed
     */
    int removeValue( const ValueType& value ) noexcept
    {
        LOCK_HASH_MAP;
        int n_removed = 0;

        for (int i_bucket = 0; i_bucket < m_impl.buckets.size(); i_bucket++)
        {
            EntryType* entry = m_impl.buckets[i_bucket];
            EntryType* prev_entry = nullptr;

            while (entry != nullptr)
            {
                if (entry->value == value)
                {
                    remove_entry_at( i_bucket, prev_entry, entry );
                    entry = prev_entry->next_entry;
                    n_removed++;
                }
                else
                {
                    prev_entry = entry;
                    entry = entry->next_entry;
                }
            }
        }

        return n_removed;
    }

    /** Remaps the hash-map to use a different number of slots for its hash function.
        Each slot corresponds to a single hash-code, and each one can contain multiple items.
        @see getNumSlots()
     */
    void remapTable( int numBuckets )
    {
        LOCK_HASH_MAP;
        m_impl.rehash( numBuckets );
    }

    /**
     * @brief get the number of buckets which are available for hashing
     *
     * Each bucket corresponds to a single hash code, and can contain multiple
     * items.
     *
     * @return number of buckets
     * @see size()
     */
    inline int numBuckets() const noexcept
    {
        return m_impl.buckets.size();
    }

    inline int numUsedBuckets() const noexcept
    {
        LOCK_HASH_MAP;
        return m_impl.num_used_buckets();
    }

    //==============================================================================
    /** Efficiently swaps the contents of two hash-maps. */
    template<class OtherHashMapType>
    void swapWith( OtherHashMapType& otherHashMap ) noexcept
    {
        LOCK_HASH_MAP;
        const typename OtherHashMapType::ScopedLockType lock2( otherHashMap.m_mutex );
        m_impl.swapWith( otherHashMap.m_impl );
    }

    //==============================================================================
    /** Returns the CriticalSection that locks this structure.
        To lock, you can call getLock().enter() and getLock().exit(), or preferably use
        an object of ScopedLockType as an RAII lock for it.
     */
    inline const MutexType& getLock() const noexcept
    {
        return m_mutex;
    }

private:
    enum { defaultHashTableSize = 101 };
    friend class Iterator;

    TableImplType m_impl;
    MutexType m_mutex;

    JUCE_LEAK_DETECTOR( HashMap )
};

} // namespace treecore

#undef LOCK_HASH_MAP

#endif   // JUCE_HASHMAP_H_INCLUDED
