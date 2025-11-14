#include "lib/ChainingHashTable.hpp"

#include <functional>
#include <algorithm>

namespace hashtable {

//---------------------------------------------------------------------------
// Constructor: pick a small default bucket count and reserve buckets.
ChainingHashTable::ChainingHashTable()
    : numEntries(0), numBuckets(8), buckets(numBuckets)
{}

// Move constructor
ChainingHashTable::ChainingHashTable(ChainingHashTable&& other) noexcept
    : buckets(std::move(other.buckets)),
      numEntries(other.numEntries),
      numBuckets(other.numBuckets)
{
    other.numEntries = 0;
    other.numBuckets = 0;
}

// Destructor
ChainingHashTable::~ChainingHashTable() = default;

ChainingHashTable& ChainingHashTable::operator=(ChainingHashTable&& other) noexcept {
    if (this != &other) {
        buckets = std::move(other.buckets);
        numEntries = other.numEntries;
        numBuckets = other.numBuckets;

        other.numEntries = 0;
        other.numBuckets = 0;
    }
    return *this;
}

void ChainingHashTable::rehash() {
    size_t newSize = std::max<size_t>(1, numBuckets * 2);
    BucketContainer newBuckets(newSize);

    // Move each entry into the new bucket vector
    for (auto& bucketList : buckets) {
        for (auto& entry : bucketList) {
            size_t index = hash(entry.key, newSize);
            newBuckets[index].push_back(std::move(entry));
        }
    }

    // Replace buckets and update count
    buckets = std::move(newBuckets);
    numBuckets = newSize;
}

size_t ChainingHashTable::hash(int64_t key, size_t size) const {
    return std::hash<int64_t>{}(key) % size;
}

size_t ChainingHashTable::size() const {
    return numEntries;
}

bool ChainingHashTable::contains(int64_t key) const {
    if (numBuckets == 0 || buckets.empty()) return false;
    size_t index = hash(key, numBuckets);
    const auto& bucket = buckets[index];
    for (const auto& e : bucket) {
        if (e.key == key) return true;
    }
    return false;
}

GenericValue& ChainingHashTable::operator[](int64_t key) {
    if (numBuckets == 0) {
        numBuckets = 8;
        buckets.resize(numBuckets);
    } else if (buckets.empty()) {
        buckets.resize(numBuckets);
    }

    size_t index = hash(key, numBuckets);
    auto& bucket = buckets[index];

    for (auto& entry : bucket) {
        if (entry.key == key) {
            return entry.value;
        }
    }

    // not found -> insert default
    bucket.emplace_back(key, GenericValue{});
    ++numEntries;
    if (numEntries > numBuckets * LOAD_FACTOR_THRESHOLD) rehash();
    return bucket.back().value;
}

GenericValue& ChainingHashTable::insert(int64_t key, GenericValue&& value) {
    if (numBuckets == 0) {
        numBuckets = 8;
        buckets.resize(numBuckets);
    } else if (buckets.empty()) {
        buckets.resize(numBuckets);
    }

    size_t index = hash(key, numBuckets);
    auto& bucket = buckets[index];

    for (auto& entry : bucket) {
        if (entry.key == key) {
            entry.value = std::move(value);
            return entry.value;
        }
    }

    bucket.emplace_back(key, std::move(value));
    ++numEntries;
    if (numEntries > numBuckets * LOAD_FACTOR_THRESHOLD) rehash();
    return bucket.back().value;
}

void ChainingHashTable::erase(int64_t key) {
    if (numBuckets == 0 || buckets.empty()) return;
    size_t index = hash(key, numBuckets);
    auto& bucket = buckets[index];

    for (auto it = bucket.begin(); it != bucket.end(); ++it) {
        if (it->key == key) {
            bucket.erase(it);
            --numEntries;
            return;
        }
    }
}

//-------------------------- iterator ------------------------------------
// Note: iterator implementation depends on header layout. Keep existing
// iterator method signatures but provide safe, simple implementations.

ChainingHashTable::iterator::iterator()
    : container(nullptr), bucketIndex(0), entryIterator()
{}

ChainingHashTable::iterator::iterator(BucketContainer* c, size_t bi, std::list<Entry>::iterator it)
    : container(c), bucketIndex(bi), entryIterator(it)
{}

ChainingHashTable::iterator ChainingHashTable::begin() {
    if (buckets.empty()) return end();
    for (size_t i = 0; i < buckets.size(); ++i) {
        if (!buckets[i].empty()) {
            return iterator(&buckets, i, buckets[i].begin());
        }
    }
    return end();
}

ChainingHashTable::iterator ChainingHashTable::end() {
    return iterator(&buckets, buckets.size(), std::list<Entry>::iterator{});
}

ChainingHashTable::iterator ChainingHashTable::find(int64_t key) {
    if (buckets.empty()) return end();
    size_t index = hash(key, numBuckets);
    for (auto it = buckets[index].begin(); it != buckets[index].end(); ++it) {
        if (it->key == key) {
            return iterator(&buckets, index, it);
        }
    }
    return end();
}

// iterator pre-increment
ChainingHashTable::iterator& ChainingHashTable::iterator::operator++() {
    if (!container) return *this;
    if (bucketIndex >= container->size()) {
        entryIterator = std::list<Entry>::iterator{};
        return *this;
    }

    auto& bucket = container->at(bucketIndex);
    if (entryIterator != bucket.end()) ++entryIterator;

    // if we reached end of current bucket, find next non-empty bucket
    while ((entryIterator == bucket.end()) && (++bucketIndex < container->size())) {
        auto& nb = container->at(bucketIndex);
        if (!nb.empty()) {
            entryIterator = nb.begin();
            return *this;
        }
    }

    if (bucketIndex >= container->size()) {
        // set to end
        entryIterator = std::list<Entry>::iterator{};
    }
    return *this;
}

void ChainingHashTable::iterator::next() {
    ++(*this);
}

ChainingHashTable::iterator ChainingHashTable::iterator::operator++(int) {
    iterator tmp = *this;
    ++(*this);
    return tmp;
}

ChainingHashTable::iterator::reference ChainingHashTable::iterator::operator*() const {
    return *entryIterator;
}

ChainingHashTable::iterator::pointer ChainingHashTable::iterator::operator->() const {
    return &(*entryIterator);
}

bool ChainingHashTable::iterator::operator==(const iterator& other) const {
    return container == other.container && bucketIndex == other.bucketIndex &&
           (container == nullptr || entryIterator == other.entryIterator);
}

bool ChainingHashTable::iterator::operator!=(const iterator& other) const {
    return !(*this == other);
}

} // namespace hashtable




