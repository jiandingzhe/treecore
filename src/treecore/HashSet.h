#ifndef TREECORE_HASH_SET_H
#define TREECORE_HASH_SET_H

#include "treecore/impl/HashImpl.h"
#include "treecore/DummyCriticalSection.h"
#include "treecore/RefCountObject.h"

#define LOCK_THIS_OBJECT const ScopedLockType _lock_this_(m_mutex)
#define LOCK_PEER_OBJECT const ScopedLockType _lock_peer_(peer.m_mutex)

class TestFramework;

namespace treecore {

template<typename KeyType,
         typename HashFunctionType = DefaultHashFunctions,
         typename MutexType = DummyCriticalSection>
class HashSet: public RefCountObject
{
protected:
    struct HashSetItem
    {
        const KeyType key;

        bool operator == (const HashSetItem& other) const
        {
            return key == other.key;
        }

        bool operator != (const HashSetItem& other) const
        {
            return key != other.key;
        }
    };

    typedef impl::HashTableBase<KeyType, HashSetItem, HashFunctionType, CriticalSectionIsDummy<MutexType>::value> TableImplType;
    typedef typename TableImplType::HashEntry EntryType;

    friend class ::TestFramework;

public:
    typedef typename MutexType::ScopedLockType ScopedLockType;

    class Iterator
    {
        friend class HashSet;
        typedef typename TableImplType::template IteratorBase<const TableImplType&, const EntryType*> ItImplType;

    public:
        Iterator(const HashSet& target): m_impl(target.m_impl)
        {
        }

        /**
         * @brief move iterator to next value
         * @return false if next value exceeds table end
         */
        bool next()
        {
            return m_impl.next();
        }

        inline bool hasContent() const noexcept
        {
            return m_impl.entry != nullptr;
        }

        inline const KeyType& content() const noexcept
        {
            treecore_assert(m_impl.entry != nullptr)
            return m_impl.entry->item.key;
        }

    protected:
        ItImplType m_impl;
    };

    HashSet(int numInitSlots = 101, HashFunctionType hashFunc = HashFunctionType())
        : m_impl(numInitSlots, hashFunc)
    {
    }

    /**
     * @brief create HashSet by copying other's contents
     */
    HashSet(const HashSet& other)
        : m_impl(other.m_impl)
    {
    }

    HashSet(HashSet&& other)
        : m_impl(std::move(other.m_impl))
    {
    }

    ~HashSet()
    {
    }

    HashSet& operator= (const HashSet& peer)
    {
        LOCK_THIS_OBJECT;
        LOCK_PEER_OBJECT;
        m_impl.clone_slots_from(peer.m_impl);
        return *this;
    }

    bool operator== (const HashSet& peer) const noexcept
    {
        LOCK_THIS_OBJECT;
        LOCK_PEER_OBJECT;
        return m_impl == peer.m_impl;
    }

    bool operator!= (const HashSet& peer) const noexcept
    {
        LOCK_THIS_OBJECT;
        LOCK_PEER_OBJECT;
        return !(m_impl == peer.m_impl);
    }

    void clear()
    {
        LOCK_THIS_OBJECT;
        m_impl.clear();
    }

    inline int size() const noexcept
    {
        return m_impl.num_entries;
    }

    bool contains(const KeyType& key) const noexcept
    {
        LOCK_THIS_OBJECT;
        int i_bucket = m_impl.bucket_index(key);
        EntryType* entry = m_impl.search_entry_at(i_bucket, key);
        return entry != nullptr;
    }

    bool insert(const KeyType& content) noexcept
    {
        LOCK_THIS_OBJECT;
        int i_bucket = m_impl.bucket_index(content);
        EntryType* entry = m_impl.search_entry_at(i_bucket, content);

        if (entry)
        {
            return false;
        }
        else
        {
            entry = m_impl.create_entry_at(i_bucket, HashSetItem{content});

            if (m_impl.high_fill_rate())
                m_impl.expand_buckets();
            return true;
        }
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
    bool select(const KeyType& content, Iterator& result) noexcept
    {
        LOCK_THIS_OBJECT;
        int i_bucket = m_impl.bucket_index(content);
        EntryType* entry = m_impl.search_entry_at(i_bucket, content);

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
    bool insertOrSelect(const KeyType& key, Iterator& result) noexcept
    {
        LOCK_THIS_OBJECT;

        // search for existing entry
        int i_bucket = m_impl.bucket_index(key);
        EntryType* entry = m_impl.search_entry_at(i_bucket, key);

        if (entry)
        {
            result.m_impl.i_bucket = i_bucket;
            result.m_impl.entry = entry;
            return false;
        }

        // create new one
        entry = m_impl.create_entry_at(i_bucket, HashSetItem{key});

        if (m_impl.high_fill_rate())
        {
            m_impl.expand_buckets();
            i_bucket  = m_impl.bucket_index(key);
        }

        result.m_impl.i_bucket = i_bucket;
        result.m_impl.entry = entry;
        return true;
    }

    bool remove(const KeyType& key) noexcept
    {
        LOCK_THIS_OBJECT;
        const int i_bucket = m_impl.bucket_index(key);
        return m_impl.remove_entry_at(i_bucket, key);
    }

    void remapTable(int numBuckets)
    {
        LOCK_THIS_OBJECT;
        m_impl.rehash(numBuckets);
    }

    int numBuckets() const noexcept
    {
        return m_impl.buckets.size();
    }

    int numUsedBuckets() const noexcept
    {
        LOCK_THIS_OBJECT;
        return m_impl.num_used_buckets();
    }

protected:

    MutexType     m_mutex;
    TableImplType m_impl;
}; // class HashSet

}

#undef LOCK_THIS_OBJECT
#undef LOCK_PEER_OBJECT

#endif // TREECORE_HASH_SET_H
