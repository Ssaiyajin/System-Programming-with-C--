#ifndef H_lib_ChainingHashTable
#define H_lib_ChainingHashTable
//---------------------------------------------------------------------------
#include "lib/GenericValue.hpp"
#include <cstdint>
#include <cstddef>
#include <vector>
#include <list>
#include <iterator>
#include <memory>
//---------------------------------------------------------------------------
namespace hashtable {
//---------------------------------------------------------------------------
struct HashEntry {
    const int64_t key; // Making 'key' a const member
    GenericValue value; // Change from object to pointer
    std::unique_ptr<HashEntry> test;
    
    // Modify the constructor to accept a pointer
    HashEntry(int64_t k, GenericValue&& v) : key(k), value(std::move(v)), test(nullptr) {}
};
class ChainingHashTable {
public:
    
    using Entry = HashEntry;
    using BucketConstIterator = typename std::list<Entry>::const_iterator;
    using BucketContainer = std::vector<std::list<Entry>>;

private:
    

    BucketContainer buckets;
    size_t numEntries = 0;
    size_t numBuckets = 16;
    const float LOAD_FACTOR_THRESHOLD = 0.5;
    //std::vector<std::list<Entry>> buckets;
    void rehash();
    size_t hash(int64_t key, size_t size) const;

public:
    ChainingHashTable();
    ~ChainingHashTable();
    ChainingHashTable(ChainingHashTable&& other) noexcept;
    ChainingHashTable& operator=(ChainingHashTable&& other) noexcept;
    // const ChainingHashTable::Entry& operator*() const;
    // const ChainingHashTable::Entry* operator->() const;
    size_t size() const;
    bool contains(int64_t key) const;
    GenericValue& operator[](int64_t key);
    GenericValue& insert(int64_t key, GenericValue&& value);
    GenericValue& insert(HashEntry entry);
    void erase(int64_t key);

    class iterator {
    public:
        using iterator_category = std::forward_iterator_tag;
        using value_type = Entry;
        using difference_type = std::ptrdiff_t;
        using pointer = Entry*;
        using reference = Entry&;
        iterator();
        iterator(BucketContainer* c, size_t bi, Entry* it);

        iterator& operator++();
        iterator operator++(int);

        pointer operator->() const;
        reference operator*() const;

        bool operator==(const iterator& other) const;
        bool operator!=(const iterator& other) const;

    private:
        BucketContainer* container;
        size_t bucketIndex;
        Entry* entryIterator; // Updated to use BucketConstIterator
        void next();
    };

    iterator begin();
    iterator end();
    iterator find(int64_t key);
};

//---------------------------------------------------------------------------
} // namespace hashtable
//---------------------------------------------------------------------------
#endif




