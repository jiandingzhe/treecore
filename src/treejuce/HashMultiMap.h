#ifndef TREEJUCE_HASH_MULTI_MAP_H
#define TREEJUCE_HASH_MULTI_MAP_H

#include "treejuce/Array.h"
#include "treejuce/ArrayRef.h"
#include "treejuce/HashFunctions.h"

TREEFACE_JUCE_NAMESPACE_BEGIN

/**
 * Values of each key are stored in an Array. So we don't allow you to remove
 * individual values.
 */
template<typename KeyType,
         typename ValueType,
         typename HashFunctionType = DefaultHashFunctions,
         typename MutexType = DummyCriticalSection>
class HashMultiMap: public Object
{
protected:
    struct Entry
    {
        Entry(KeyType key, Entry* next): key(key), next(next) {}

        KeyType key;
        Array<ValueType> values;
        Entry* next;
    };

public:
    typedef typename MutexType::ScopedLockType ScopedLockType;

    class Iterator
    {
    public:
        Iterator(const HashMultiMap& target): target(target)
        {
        }

        bool next()
        {
            // move from initial state
            if (i_slot == -1)
            {
                return move_to_next_valid_slot();
            }
            else
            {
                // move from previous position
                if (entry)
                {
                    return move_to_next_valid_entry();
                }
                // we have arrived tail
                else
                {
                    return false;
                }
            }
        }

        KeyType getKey() const NOEXCEPT
        {
            return entry->key;
        }

        ArrayRef<ValueType> getValues() NOEXCEPT
        {
            return ArrayRef<ValueType>(entry->values);
        }

    protected:

        inline bool move_to_next_valid_entry() NOEXCEPT
        {
            entry = entry->next;
            if (entry)
            {
                return true;
            }
            else
            {
                return move_to_next_valid_slot();
            }
        }

        inline bool move_to_next_valid_slot() NOEXCEPT
        {
            while (1)
            {
                i_slot++;
                if (i_slot >= target.m_slots.size())
                    return false;

                entry = target.m_slots.getUnchecked(i_slot);
                if (entry)
                {
                    return true;
                }
            }
        }

        const HashMultiMap& target;
        int i_slot = -1;
        typename HashMultiMap::Entry* entry = nullptr;
    };

    HashMultiMap(int n_init_slots = 101, HashFunctionType hash_func = HashFunctionType())
        : m_hash_func(hash_func)
    {
        m_slots.insertMultiple(0, nullptr, n_init_slots);
    }

    HashMultiMap(HashMultiMap&& other)
        : m_hash_func(other.m_hash_func)
        , m_size(other.m_size)
        , m_slots(std::move(other.m_slots))
    {
        other.m_size = 0;
    }

    JUCE_DECLARE_NON_COPYABLE(HashMultiMap);

    ~HashMultiMap()
    {
        clear();
    }

    void clear()
    {
        const ScopedLockType lock(m_mutex);

        for (int i = m_slots.size() - 1; i >= 0; i--)
        {
            Entry* curr_bucket = m_slots.getUnchecked(i);

            while (curr_bucket != nullptr)
            {
                Entry* next_bucket = curr_bucket->next;
                delete curr_bucket;
                curr_bucket = next_bucket;
            }

            m_slots.setUnchecked(i, nullptr);
        }

        m_size = 0;
    }


    /**
     * @brief get the number of items in hash map
     * @return number of items
     */
    inline int size() const NOEXCEPT
    {
        return m_size;
    }

    /**
     * @brief test whether hash map contains this key
     * @param key
     * @return true if has this key, otherwise false
     */
    bool contains(KeyType key) const NOEXCEPT
    {
        const ScopedLockType lock(m_mutex);

        int hash_result = generateHashFor(key);
        for (const Entry* entry = m_slots.getUnchecked(hash_result); entry != nullptr; entry = entry->next)
        {
            if (entry->key == key)
                return true;
        }

        return false;
    }

    /**
     * @brief count the number of items under this key
     * @param key
     * @return number of items
     */
    int count(KeyType key) const NOEXCEPT
    {
        const ScopedLockType lock(m_mutex);

        int hash_result = generateHashFor(key);
        for (const Entry* entry = m_slots.getUnchecked(hash_result); entry != nullptr; entry = entry->next)
        {
            if (entry->key == key)
                return entry->values.size();
        }

        return 0;
    }

    int getNumKeys() const NOEXCEPT
    {
        int n_key = 0;

        for (int i_slot = 0; i_slot < m_slots.size(); i_slot++)
        {
            Entry* first_entry = m_slots.getUnchecked(i_slot);
            if (first_entry == nullptr)
                continue;

            for (Entry* entry = first_entry; entry != nullptr; entry = entry->next)
                n_key++;
        }

        return n_key;
    }

    /**
     * @brief insert a value into hash map
     * @param key
     * @param value
     */
    void insert(KeyType key, ValueType value)
    {
        const ScopedLockType lock(m_mutex);

        int hash_result = generateHashFor(key);
        Entry* const first_entry = m_slots.getUnchecked(hash_result);

        // try to append to existing entry
        for (Entry* entry = first_entry; entry != nullptr; entry = entry->next)
        {
            if (entry->key == key)
            {
                entry->values.add(value);
                m_size++;
                return;
            }
        }

        // create a new entry for this key
        Entry* new_entry = new Entry(key, first_entry);
        m_slots.setUnchecked(hash_result, new_entry);
        new_entry->values.add(value);
        m_size++;

        // resize slots if too many things filled
        if (float(m_size) / float(m_slots.size()) > 1.5)
        {
            remapTable(m_slots.size() * 2);
        }
    }

    /**
     * @brief get values by specified key
     * @param key
     * @return a list of values that can be read
     */
    const ArrayRef<ValueType> operator [] (KeyType key)
    {
        const ScopedLockType lock(m_mutex);

        int hash_result = generateHashFor(key);
        Entry* const first_entry = m_slots.getUnchecked(hash_result);

        // try to find existing key
        for (Entry* entry = first_entry; entry != nullptr; entry = entry->next)
        {
            if (entry->key == key)
                return ArrayRef<ValueType>(entry->values);
        }

        return ArrayRef<ValueType>();
    }

    /**
     * @brief remove all items of this key
     * @param key: the key to be removed
     * @return number of items removed
     */
    int remove(KeyType key)
    {
        const ScopedLockType lock(m_mutex);

        int hash_result = generateHashFor(key);
        Entry* entry = m_slots.getUnchecked(hash_result);
        Entry* prev = nullptr;

        while (entry != nullptr)
        {
            if (entry->key == key)
            {
                int n_remove = entry->values.size();
                m_size -= n_remove;

                Entry* next = entry->next;
                if (prev == nullptr)
                {
                    m_slots.setUnchecked(hash_result, next);
                }
                else
                {
                    prev->next = next;
                }

                delete entry;
                return n_remove;
            }
            else
            {
                prev = entry;
                entry = entry->next;
            }
        }

        return 0;
    }

    void remapTable(int new_slot_size)
    {
        Array<Entry*> slots_new;
        slots_new.insertMultiple(0, nullptr, new_slot_size);

        for (int i_slot_old = 0; i_slot_old < m_slots.size(); i_slot_old++)
        {
            Entry* entry = m_slots.getUnchecked(i_slot_old);
            Entry* next_entry = nullptr;
            while (entry != nullptr)
            {
                next_entry = entry->next;
                int hash = m_hash_func.generateHash(entry->key, new_slot_size);

                entry->next = slots_new.getUnchecked(hash);
                slots_new.setUnchecked(hash, entry);
                entry = next_entry;
            }
        }

        m_slots.swapWith(slots_new);
    }

    inline int getNumSlots() const NOEXCEPT
    {
        return m_slots.size();
    }

    inline int getNumUsedSlots() const NOEXCEPT
    {
        int n_used = 0;
        for (int i = 0; i < m_slots.size(); i++)
        {
            if (m_slots.getUnchecked(i) != nullptr)
                n_used++;
        }
        return n_used;
    }

protected:

    inline int generateHashFor (KeyType key) const
    {
        const int hash = m_hash_func.generateHash (key, m_slots.size());
        jassert (isPositiveAndBelow (hash, getNumSlots())); // your hash function is generating out-of-range numbers!
        return hash;
    }

    HashFunctionType m_hash_func;
    Array<Entry*>    m_slots;
    int              m_size = 0;
    MutexType        m_mutex;
};

TREEFACE_JUCE_NAMESPACE_END

#endif // TREEJUCE_HASH_MULTI_MAP_H
