#ifndef TREECORE_HASH_SET_H
#define TREECORE_HASH_SET_H

#include "treecore/Array.h"
#include "treecore/HashFunctions.h"
#include "treecore/ObjectPool.h"
#include "treecore/RefCountObject.h"
#include "treecore/RefCountSingleton.h"

namespace treecore {

template<typename KeyType,
         typename HashFunctionType = DefaultHashFunctions,
         typename MutexType = DummyCriticalSection>
class HashSet: public RefCountObject
{
protected:
    struct Entry
    {
        Entry(KeyType key, Entry* next): key(key), next(next)
        {
        }

        KeyType key;
        Entry* next;
    };

    // FIXME singleton is not released
    typedef RefCountSingleton<ObjectPool<Entry, !CriticalSectionIsDummy<MutexType>::value> > EntryPoolType;

public:
    typedef typename MutexType::ScopedLockType ScopedLockType;

    class Iterator
    {
        friend class HashSet;

    public:
        Iterator(const HashSet& target): target(target)
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

        inline bool hasValue() const noexcept
        {
            return entry != nullptr;
        }

        inline const KeyType& get() const noexcept
        {
            return entry->key;
        }

    protected:
        inline bool move_to_next_valid_entry() noexcept
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

        inline bool move_to_next_valid_slot() noexcept
        {
            while (1)
            {
                i_slot++;
                if (i_slot >= target.m_slots.size())
                {
                    entry = nullptr;
                    return false;
                }

                entry = target.m_slots.getUnchecked(i_slot);
                if (entry)
                    return true;
            }
        }

        const HashSet& target;
        int i_slot = -1;
        const typename HashSet::Entry* entry = nullptr;
    };

    HashSet(int n_initial_slots = 101, HashFunctionType hash_func = HashFunctionType())
        : m_hash_func(hash_func)
    {
        m_slots.insertMultiple(0, nullptr, n_initial_slots);
    }

    ~HashSet()
    {
        clear();
    }

    void clear()
    {
        const ScopedLockType lock(m_mutex);

        for (int i = m_slots.size()-1 ; i >= 0; i--)
        {
            Entry* curr_bucket = m_slots.getUnchecked(i);

            while (curr_bucket != nullptr)
            {
                Entry* next_bucket = curr_bucket->next;
                EntryPoolType::getInstance()->recycle(curr_bucket);
                curr_bucket = next_bucket;
            }

            m_slots.setUnchecked(i, nullptr);
        }

        m_size = 0;
    }

    inline int size() const noexcept
    {
        return m_size;
    }

    bool contains(KeyType key) const noexcept
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

    bool search(const KeyType& key, Iterator& result) noexcept
    {
        const ScopedLockType lock(m_mutex);
        int hash_result = generateHashFor(key);
        for (const Entry* entry = m_slots.getUnchecked(hash_result); entry != nullptr; entry = entry->next)
        {
            if (entry->key == key)
            {
                result.entry = entry;
                result.i_slot = hash_result;
            }
        }

        return false;
    }

    int getNumKeys() const noexcept
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
     * @brief Insert a value into hash set, and get iterator to the newly added
     *        value or the already existing one.
     *
     * @param key the value to be inserted
     * @param result will point to newly added value or already existing value
     * @return true if key is inserted, false if already exists
     */
    bool insertAndGet(const KeyType& key, Iterator& result)
    {
        const ScopedLockType lock(m_mutex);

        int hash_result = generateHashFor(key);
        Entry* const first_entry = m_slots.getUnchecked(hash_result);

        // find if already exists
        for (Entry* entry = first_entry; entry != nullptr; entry = entry->next)
        {
            if (entry->key == key)
            {
                result.i_slot = hash_result;
                result.entry = entry;
                return false;
            }
        }

        // create a new entry for this key
        Entry* new_entry = EntryPoolType::getInstance()->generate(key, first_entry);
        m_slots.setUnchecked(hash_result, new_entry);
        m_size++;

        // rehash when needed
        if (float(m_size) / float(m_slots.size()) > 1.5)
        {
            remapTable(m_slots.size() * 2);
            hash_result = generateHashFor(key);
        }

        result.i_slot = hash_result;
        result.entry = new_entry;
        return true;
    }

    bool insert(KeyType key)
    {
        const ScopedLockType lock(m_mutex);

        int hash_result = generateHashFor(key);
        Entry* const first_entry = m_slots.getUnchecked(hash_result);

        // find duplications
        for (Entry* entry = first_entry; entry != nullptr; entry = entry->next)
        {
            if (entry->key == key)
                return false;
        }

        // create a new entry for this key
        Entry* new_entry = EntryPoolType::getInstance()->generate(key, first_entry);
        m_slots.setUnchecked(hash_result, new_entry);
        m_size++;

        // resize slots if too many things filled
        if (float(m_size) / float(m_slots.size()) > 1.5)
        {
            remapTable(m_slots.size() * 2);
        }

        return true;
    }

    bool remove(KeyType key)
    {
        const ScopedLockType lock(m_mutex);

        int hash_result = generateHashFor(key);
        Entry* entry = m_slots.getUnchecked(hash_result);
        Entry* prev = nullptr;

        while (entry != nullptr)
        {
            if (entry->key == key)
            {
                Entry* next = entry->next;
                if (prev == nullptr)
                {
                    m_slots.setUnchecked(hash_result, next);
                }
                else
                {
                    prev->next = next;
                }

                EntryPoolType::getInstance()->recycle(entry);
                m_size--;
                return true;
            }
            else
            {
                prev = entry;
                entry = entry->next;
            }
        }

        return false;
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

    inline int getNumSlots() const noexcept
    {
        return m_slots.size();
    }

    inline int getNumUsedSlots() const noexcept
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
    int generateHashFor (KeyType key) const
    {
        const int hash = m_hash_func.generateHash (key, getNumSlots());
        jassert (isPositiveAndBelow (hash, getNumSlots())); // your hash function is generating out-of-range numbers!
        return hash;
    }

    HashFunctionType m_hash_func;
    Array<Entry*>    m_slots;
    int              m_size = 0;
    MutexType        m_mutex;
}; // class HashSet

}

#endif // TREECORE_HASH_SET_H
