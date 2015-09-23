#ifndef TREECORE_HASH_SET_H
#define TREECORE_HASH_SET_H

#include "treecore/Array.h"
#include "treecore/HashFunctions.h"
#include "treecore/RefCountObject.h"

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

public:
    typedef typename MutexType::ScopedLockType ScopedLockType;

    class Iterator
    {
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

        inline KeyType get() const NOEXCEPT
        {
            return entry->key;
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
                delete curr_bucket;
                curr_bucket = next_bucket;
            }

            m_slots.setUnchecked(i, nullptr);
        }

        m_size = 0;
    }

    inline int size() const NOEXCEPT
    {
        return m_size;
    }

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
        Entry* new_entry = new Entry(key, first_entry);
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

                delete entry;
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
