#ifndef H_lib_ChainingHashTable
#define H_lib_ChainingHashTable
//---------------------------------------------------------------------------

#include "lib/GenericValue.hpp"
#include <cstdint>
#include <cstddef>
#include <vector>
#include <list>
#include <iterator>
#include <functional>
#include <utility>

//---------------------------------------------------------------------------

namespace hashtable {
//---------------------------------------------------------------------------

struct HashEntry {
    const int64_t key;
    GenericValue value;

    HashEntry(int64_t k, GenericValue&& v) : key(k), value(std::move(v)) {}

    // disable copy (GenericValue is non-copyable)
    HashEntry(const HashEntry&) = delete;
    HashEntry& operator=(const HashEntry&) = delete;

    // allow move
    HashEntry(HashEntry&& other) noexcept : key(other.key), value(std::move(other.value)) {}
    HashEntry& operator=(HashEntry&&) = default;
};

class ChainingHashTable {
public:
    using Entry = HashEntry;
    using Bucket = std::list<Entry>;
    using BucketContainer = std::vector<Bucket>;
    using BucketIterator = typename Bucket::iterator;

private:
    BucketContainer buckets;
    size_t numEntries = 0;
    size_t numBuckets = 16;
    const float LOAD_FACTOR_THRESHOLD = 0.5f;

    void rehash();
    size_t hashIndex(int64_t key, size_t bucketCount) const;

public:
    ChainingHashTable();
    ~ChainingHashTable();
    ChainingHashTable(ChainingHashTable&& other) noexcept;
    ChainingHashTable& operator=(ChainingHashTable&& other) noexcept;

    size_t size() const;
    bool contains(int64_t key) const;
    GenericValue& operator[](int64_t key);
    GenericValue& insert(int64_t key, GenericValue&& value);
    void erase(int64_t key);

    class iterator {
    public:
        using iterator_category = std::forward_iterator_tag;
        using value_type = Entry;
        using difference_type = std::ptrdiff_t;
        using pointer = Entry*;
        using reference = Entry&;

        iterator();
        iterator(BucketContainer* container, size_t bucketIndex, BucketIterator it);

        iterator& operator++();
        iterator operator++(int);

        pointer operator->() const;
        reference operator*() const;

        bool operator==(const iterator& other) const;
        bool operator!=(const iterator& other) const;

    private:
        BucketContainer* container = nullptr;
        size_t bucketIndex = 0;
        BucketIterator entryIt;
        void advance_to_next_valid();
        friend class ChainingHashTable;
    };

    iterator begin();
    iterator end();
    iterator find(int64_t key);
};

//---------------------------------------------------------------------------

} // namespace hashtable
//---------------------------------------------------------------------------

#endif
