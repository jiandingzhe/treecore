#ifndef TREECORE_IMPL_HASH_H
#define TREECORE_IMPL_HASH_H

#include "treecore/Array.h"
#include "treecore/HashFunctions.h"
#include "treecore/MPL.h"
#include "treecore/ObjectPool.h"

#define TREECORE_REHASH_CUTOFF 1.5

namespace treecore
{
namespace impl
{

/**
 * @brief base class to implement hash storages
 *
 * This class contain infrastructures for implement hash table based storages,
 * such as map, multimap and set. User should provide ItemType which contains a
 * member variable named key and is type KeyType.
 *
 * For example:
 *
 * @code
 * struct MyItemType
 * {
 *     int key;
 *     float foo;
 *     float bar;
 *     int baz;
 * };
 *
 * class MyUtility
 * {
 *     HashTableBase<int, MyItemType> impl;
 * };
 * @endcode
 *
 * @see HashMap
 * @see HashMultiMap
 * @see HashSet
 */
template<typename KeyType,
         typename ItemType,
         typename HashFuncType = DefaultHashFunctions,
         bool poolIsThreaded = false>
struct HashTableBase
{
    struct HashEntry
    {
        HashEntry(const ItemType& item, HashEntry* next)
            : item(item)
            , next_entry(next)
        {
        }

        HashEntry(ItemType& item, HashEntry* next)
            : item(std::move(item))
            , next_entry(next)
        {
        }

        ItemType   item{};
        HashEntry* next_entry = nullptr;
    };

    typedef ObjectPool<HashEntry, poolIsThreaded> EntryPoolType;

    template<typename TableRefType, typename _EntryPtrType>
    struct IteratorBase
    {
        typedef _EntryPtrType EntryPtrType;
        
        IteratorBase(TableRefType& table): table(table)
        {
        }
        
        bool next() noexcept
        {
            if (i_bucket == -1)
            {
                return move_to_next_valid_slot();
            }
            else
            {
                if (entry)
                    return move_to_next_valid_entry();
                else
                    return false;
            }
        }
        
        bool move_to_next_valid_entry() noexcept
        {
            entry = entry->next_entry;
            if (entry)
            {
                return true;
            }
            else
            {
                return move_to_next_valid_slot();
            }
        }

        bool move_to_next_valid_slot() noexcept
        {
            while (1)
            {
                i_bucket++;
                if (i_bucket >= table.buckets.size())
                    return false;

                entry = table.buckets[i_bucket];
                if (entry)
                    return true;
            }
        }
        
        TableRefType table;
        EntryPtrType entry    = nullptr;
        int          i_bucket = -1;
    };

    HashTableBase(int num_init_buckets, HashFuncType hash_func) noexcept
        : hash_func(hash_func)
    {
        buckets.insertMultiple(0, nullptr, num_init_buckets);
    }

    HashTableBase(const HashTableBase& other) noexcept
    {
        clone_slots_from(other);
    }

    HashTableBase(HashTableBase&& other) noexcept
        : buckets(std::move(other.buckets))
        , num_entries(other.num_entries)
    {
        other.num_entries = 0;
    }

    ~HashTableBase() noexcept
    {
        clear();
    }

    bool operator== (const HashTableBase& other) const noexcept
    {
        if (num_entries != other.num_entries)
            return false;

        for (int i_bucket = 0; i_bucket < buckets.size(); i_bucket++)
        {
            for (const HashEntry* entry = buckets[i_bucket]; entry != nullptr; entry = entry->next_entry)
            {
                const KeyType& key = entry->item.key;
                int i_other = other.bucket_index(key);
                const HashEntry* other_entry = other.search_entry_at(i_other, key);

                if (!other_entry || entry->item != other_entry->item)
                    return false;
            }
        }

        return true;
    }

    void get_all_keys(Array<KeyType>& result) const
    {
        result.clear();
        result.ensureStorageAllocated(num_entries);

        for (int i_bucket = 0; i_bucket < buckets.size(); i_bucket++)
        {
            for (HashEntry* entry = buckets[i_bucket]; entry != nullptr; entry = entry->next_entry)
                result.add(entry->item.key);
        }
    }

    HashEntry* search_entry_at(int i_bucket, const KeyType& key) const noexcept
    {
        for (const HashEntry* entry = buckets[i_bucket]; entry != nullptr; entry = entry->next_entry)
        {
            if (entry->item.key == key)
                return const_cast<HashEntry*>(entry);
        }

        return nullptr;
    }

    void search_entry_and_prev_at(int i_bucket, const KeyType& key, HashEntry*& prev, HashEntry*& entry) const noexcept
    {
        entry = buckets[i_bucket];
        prev = nullptr;

        while (entry != nullptr)
        {
            if (entry->item.key == key)
                return;

            prev = entry;
            entry = entry->next_entry;
        }
    }

    bool high_fill_rate() const noexcept
    {
        return float(num_entries) / float(buckets.size()) > TREECORE_REHASH_CUTOFF;
    }

    template<typename T1,typename T2>
    void fuck(T1 a,T2 b)
    {}

    HashEntry* create_entry_at(int i_bucket, ItemType&& item)
    {
        HashEntry* entry = EntryPoolType::getInstance()->generate(std::move(item), buckets[i_bucket]);
        buckets[i_bucket] = entry;
        num_entries++;
        return entry;
    }

    bool remove_entry_at(int i_bucket, const KeyType& key) noexcept
    {
        HashEntry* entry = nullptr;
        HashEntry* prev_entry = nullptr;
        search_entry_and_prev_at(i_bucket, key, prev_entry,entry);

        if (entry)
            remove_entry(i_bucket, prev_entry, entry);

        return entry != nullptr;
    }

    void remove_entry(int i_bucket, HashEntry* prev_entry, HashEntry* entry)
    {
        HashEntry* next_entry = entry->next_entry;

        if (prev_entry)
            prev_entry->next_entry = next_entry;
        else
            buckets[i_bucket] = next_entry;

        EntryPoolType::getInstance()->recycle(entry);
        num_entries--;
    }

    void rehash(int new_size) noexcept
    {
        Array<HashEntry*> new_buckets;
        new_buckets.insertMultiple(0, nullptr, new_size);

        // traverse all entries
        // rehash them and fill to new buckets
        for (int i_bucket_old = 0; i_bucket_old < buckets.size(); i_bucket_old++)
        {
            HashEntry* entry = buckets[i_bucket_old];
            while (entry != nullptr)
            {
                HashEntry* old_next = entry->next_entry;

                int i_bucket_new = hash_func.generateHash(entry->item.key, new_size);
                entry->next_entry = new_buckets[i_bucket_new];
                new_buckets[i_bucket_new] = entry;

                entry = old_next;
            }
        }

        // swap bucket storage
        buckets.swapWith(new_buckets);
    }

    void expand_buckets()
    {
        rehash(buckets.size() * 2);
    }

    void clear()
    {
        for (int i = buckets.size(); --i >= 0;)
        {
            HashEntry* entry = buckets[i];
            buckets[i] = nullptr;

            while (entry != nullptr)
            {
                HashEntry* next_entry = entry->next_entry;
                EntryPoolType::getInstance()->recycle(entry);
                entry = next_entry;
            }
        }

        num_entries = 0;
    }

    void clone_slots_from(const HashTableBase& other)
    {
        int num_slots = other.buckets.size();
        buckets.clearQuick();
        buckets.ensureStorageAllocated(num_slots);

        for (int hashcode = 0; hashcode < num_slots; hashcode++)
        {
            HashEntry* other_first_entry = other.buckets[hashcode];
            buckets.add(nullptr);

            // traverse current bucket's all entries in other table
            // and copy them into the bucket in this table
            for (HashEntry* other_entry = other_first_entry; other_entry != nullptr; other_entry = other_entry->next_entry)
            {
                HashEntry* my_entry = EntryPoolType::getInstance()->generate(other_entry->item, buckets[hashcode]);
                buckets[hashcode] = my_entry;
            }
        }
    }

    int bucket_index(const KeyType& key) const noexcept
    {
        return hash_func.generateHash(key, buckets.size());
    }

    int num_used_buckets() const noexcept
    {
        int n_used = 0;
        for (int i = 0; i < buckets.size(); i++)
        {
            if (buckets[i])
                n_used++;
        }
        return n_used;
    }

    template<typename OtherTableType>
    void swapWith(OtherTableType& other) noexcept
    {
        buckets.swapWith(other.buckets);
        std::swap(num_entries, other.num_entries);
    }

    int               num_entries = 0;
    HashFuncType      hash_func;
    Array<HashEntry*> buckets;
}; // class HashTableBase

} // namespace impl
} // namespace treecore

#endif // TREECORE_IMPL_HASH_H
