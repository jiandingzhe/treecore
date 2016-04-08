#ifndef TREECORE_HASH_MULTI_MAP_H
#define TREECORE_HASH_MULTI_MAP_H

#include "treecore/Array.h"
#include "treecore/ArrayRef.h"
#include "treecore/DummyCriticalSection.h"
#include "treecore/ObjectPool.h"
#include "treecore/RefCountObject.h"
#include "treecore/RefCountSingleton.h"

#include "treecore/impl/HashImpl.h"

#define LOCK_THIS_OBJECT const ScopedLockType _lock_this_(m_mutex)
#define LOCK_PEER_OBJECT const ScopedLockType _lock_peer_(peer.m_mutex)

namespace treecore {

/**
 * Values of each key are stored in an Array. So we don't allow you to remove
 * individual values.
 */
template<typename KeyType,
         typename ValueType,
         typename HashFunctionType = DefaultHashFunctions,
         typename MutexType = DummyCriticalSection>
class HashMultiMap: public RefCountObject
{
    struct MultiItem
    {
        KeyType key;
        Array<ValueType> values;

        bool operator == (const MultiItem& other) const
        {
            return key == other.key && values == other.values;
        }

        bool operator != (const MultiItem& other) const
        {
            return key != other.key || values != other.values;
        }
    };

    typedef impl::HashTableBase<KeyType, MultiItem, HashFunctionType, CriticalSectionIsDummy<MutexType>::value> TableImplType;
    typedef typename TableImplType::HashEntry EntryType;

public:
    typedef typename MutexType::ScopedLockType ScopedLockType;

    class Iterator
    {
        friend class HashMultiMap;
        typedef typename TableImplType::template IteratorBase<TableImplType&, EntryType*> ItImplType;

    public:
        Iterator(HashMultiMap& target): m_impl(target.m_impl)
        {
        }

        bool next()
        {
            // initial state
            if (m_impl.i_bucket == -1)
                return m_impl.move_to_next_valid_slot();

            // we have arrived very tail
            if (!m_impl.entry)
                return false;

            return move_to_next_valid_item();
        }

        bool nextKey()
        {
            m_i_value = 0;
            // initial state
            if (m_impl.i_bucket == -1)
                return m_impl.move_to_next_valid_slot();

            // we have arrived very tail
            if (!m_impl.entry)
                return false;

            return m_impl.move_to_next_valid_entry();
        }

        bool hasContent() const noexcept
        {
            return m_impl.entry != nullptr;
        }

        KeyType& key() noexcept
        {
            treecore_assert(m_impl.entry != nullptr);
            return m_impl.entry->item.key;
        }

        ValueType& value() noexcept
        {
            treecore_assert(m_impl.entry != nullptr);
            return m_impl.entry->item.values[m_i_value];
        }

        Array<ValueType>& values() noexcept
        {
            treecore_assert(m_impl.entry != nullptr);
            return m_impl.entry->item.values;
        }

        int numValuesForCurrentKey() const noexcept
        {
            treecore_assert(m_impl.entry != nullptr);
            return m_impl.entry->item.values.size();
        }

    protected:
        bool move_to_next_valid_item()
        {
            m_i_value++;

            // arrived to tail of current entry, move to next entry
            if (m_i_value >= m_impl.entry->item.values.size())
            {
                m_i_value = 0;
                return m_impl.move_to_next_valid_entry();
            }

            return true;
        }

        int m_i_value = 0;
        ItImplType m_impl;

    };

    HashMultiMap(int initBuckets = 101, HashFunctionType hashFunc = HashFunctionType())
        : m_impl(initBuckets, hashFunc)
    {
    }

    HashMultiMap(const HashMultiMap& other)
        : m_impl(other.m_impl)
        , m_num_values(other.m_num_values)
    {
    }

    HashMultiMap(HashMultiMap&& other)
        : m_impl(std::move(other.m_impl))
        , m_num_values(other.m_num_values)
    {
        other.m_num_values = 0;
    }

    virtual ~HashMultiMap()
    {
    }

    void clear() noexcept
    {
        LOCK_THIS_OBJECT;
        m_impl.clear();
        m_num_values = 0;
    }


    /**
     * @brief get the number of items in hash map
     * @return number of items
     * @see getNumKeys()
     */
    inline int size() const noexcept
    {
        return m_num_values;
    }

    /**
     * @brief get all keys in this table
     * @return an array containing all keys
     */
    Array<KeyType> getAllKeys() const
    {
        Array<KeyType> re;
        m_impl.get_all_keys(re);
        return re;
    }

    /**
     * @brief test whether hash map contains this key
     * @param key
     * @return true if has this key, otherwise false
     */
    bool contains(const KeyType& key) const noexcept
    {
        LOCK_THIS_OBJECT;
        int i_bucket = m_impl.bucket_index(key);
        EntryType* entry = m_impl.search_entry_at(i_bucket, key);
        return entry != nullptr;
    }

    bool contains(const KeyType& key, const ValueType& value) const noexcept
    {
        LOCK_THIS_OBJECT;

        int hash_result = m_impl.bucket_index(key);
        EntryType* entry = m_impl.search_entry_at(hash_result, key);

        if (entry)
        {
            return entry->item.values.contains(value);
        }
        else
        {
            return false;
        }
    }

    /**
     * @brief whether the value exists in it
     *
     * This operation would traverse the whole table and is slow.
     *
     * @param value value to be searched
     * @return true if
     */
    bool containsValue(const ValueType& value) const noexcept
    {
        LOCK_THIS_OBJECT;

        for (int i = 0; i < m_impl.buckets.size(); i++)
        {
            for (const EntryType* entry = m_impl.buckets[i]; entry != nullptr; entry = entry->next_entry)
            {
                if (entry->item.values.contains(value))
                    return true;
            }
        }

        return false;
    }

    /**
     * @brief count the number of items under this key
     * @param key
     * @return number of items
     */
    int count(const KeyType& key) const noexcept
    {
        LOCK_THIS_OBJECT;

        int hash_result = m_impl.bucket_index(key);
        EntryType* entry = m_impl.search_entry_at(hash_result, key);

        if (entry)
            return entry->item.values.size();
        else
            return 0;
    }

    /**
     * @brief Search table by key, and make iterator point to the item stored
     *        by this key.
     *
     * @param key     key to search for
     * @param result  This iterator will be modified to point to search result.
     *                If not found, it will not be modified.
     *
     * @return true if got key, false if not
     */
    bool select(const KeyType& key, Iterator& result) noexcept
    {
        LOCK_THIS_OBJECT;
        int hash_result = m_impl.bucket_index(key);
        EntryType* entry = m_impl.search_entry_at(hash_result, key);

        if (entry)
        {
            result.m_i_value = 0;
            result.m_impl.entry = entry;
            result.m_impl.i_bucket = hash_result;
            return true;
        }

        return false;
    }

    /**
     * @brief get the number of different keys in this table
     * @return number of keys
     * @see size()
     */
    int numKeys() const noexcept
    {
        return m_impl.num_entries;
    }

    /**
     * @brief store a value into table using specified key
     * @param key
     * @param value
     */
    void store(const KeyType& key, const ValueType& value) noexcept
    {
        LOCK_THIS_OBJECT;
        int i_bucket = m_impl.bucket_index(key);

        // try to get existing entry, or create new entry
        EntryType* entry = m_impl.search_entry_at(i_bucket, key);

        if (!entry)
        {
            entry = m_impl.create_entry_at(i_bucket, MultiItem{key});

            if (m_impl.high_fill_rate())
                m_impl.expand_buckets();
        }

        // store value
        entry->item.values.add(value);
        m_num_values++;
    }

    /**
     * @brief Store a value using specified key, and make iterator point to it.
     *
     * @param key
     * @param value
     * @param result  iterator will be set to point to the newly inserted item
     */
    void store(const KeyType& key, const ValueType& value, Iterator& result)
    {
        LOCK_THIS_OBJECT;
        int i_bucket = m_impl.bucket_index(key);

        // try to get existing entry or create new entry
        EntryType* entry = m_impl.search_entry_at(i_bucket, key);

        if (entry)
        {
            entry = m_impl.create_entry_at(i_bucket, MultiItem{key});

            if (m_impl.high_fill_rate())
            {
                m_impl.expand_buckets();
                i_bucket = m_impl.bucket_index(key);
            }
        }

        // store value
        entry->item.values.add(value);
        m_num_values++;

        // fill result iterator
        result.m_impl.entry = entry;
        result.m_impl.i_bucket = i_bucket;
        result.m_i_value = entry->item.values.size() - 1;
    }

    /**
     * @brief remove all items stored by this key
     *
     * @param key: the key to be removed
     *
     * @return number of items removed
     */
    int remove(KeyType key) noexcept
    {
        LOCK_THIS_OBJECT;

        const int i_bucket = m_impl.bucket_index(key);
        EntryType* entry = nullptr;
        EntryType* prev  = nullptr;
        m_impl.search_entry_and_prev_at(i_bucket, key, prev, entry);

        if (entry)
        {
            int num_remove = entry->item.values.size();
            m_impl.remove_entry(i_bucket, prev, entry);
            m_num_values -= num_remove;
            return num_remove;
        }
        else
        {
            return 0;
        }
    }

    void remapTable(int numBuckets) noexcept
    {
        LOCK_THIS_OBJECT;
        m_impl.rehash(numBuckets);
    }

    inline int numBuckets() const noexcept
    {
        return m_impl.buckets.size();
    }

    inline int numUsedBuckets() const noexcept
    {
        return m_impl.num_used_buckets();
    }

protected:

    int            m_num_values = 0;
    TableImplType  m_impl;
    MutexType      m_mutex;
};

}

#undef LOCK_THIS_OBJECT
#undef LOCK_PEER_OBJECT

#endif // TREECORE_HASH_MULTI_MAP_H
