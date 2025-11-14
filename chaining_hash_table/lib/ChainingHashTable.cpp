#include "lib/ChainingHashTable.hpp"
#include <algorithm>
#include <stdexcept>

//---------------------------------------------------------------------------

namespace hashtable {
//---------------------------------------------------------------------------

ChainingHashTable::ChainingHashTable()
    : buckets(numBuckets), numEntries(0) {}

ChainingHashTable::~ChainingHashTable() {
    // default containers destructors are fine
    buckets.clear();
    numEntries = 0;
    numBuckets = 0;
}

ChainingHashTable::ChainingHashTable(ChainingHashTable&& other) noexcept
    : buckets(std::move(other.buckets)),
      numEntries(other.numEntries),
      numBuckets(other.numBuckets) {
    other.numEntries = 0;
    other.numBuckets = 0;
}

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

    for (auto& bucket : buckets) {
        for (auto& entry : bucket) {
            size_t idx = hashIndex(entry.key, newSize);
            // Move value into new bucket
            newBuckets[idx].emplace_back(entry.key, std::move(entry.value));
        }
    }

    // Replace buckets atomically with move; old buckets get destroyed here
    buckets = std::move(newBuckets);
    numBuckets = newSize;
    // numEntries stays the same
}

size_t ChainingHashTable::hashIndex(int64_t key, size_t bucketCount) const {
    return std::hash<int64_t>{}(key) % bucketCount;
}

size_t ChainingHashTable::size() const {
    return numEntries;
}

bool ChainingHashTable::contains(int64_t key) const {
    if (buckets.empty() || numBuckets == 0) return false;
    size_t idx = hashIndex(key, numBuckets);
    const auto& bucket = buckets[idx];
    for (const auto& e : bucket) {
        if (e.key == key) return true;
    }
    return false;
}

GenericValue& ChainingHashTable::operator[](int64_t key) {
    // Ensure buckets exist
    if (buckets.empty()) {
        buckets.resize(numBuckets);
    }

    // If rehash will be needed after insertion, do it now BEFORE taking any references
    if ((numEntries + 1) > static_cast<size_t>(numBuckets * LOAD_FACTOR_THRESHOLD)) {
        rehash();
    }

    // Now use current buckets safely
    size_t idx = hashIndex(key, numBuckets);
    auto& bucket = buckets[idx];

    for (auto it = bucket.begin(); it != bucket.end(); ++it) {
        if (it->key == key) {
            return it->value;
        }
    }

    // Insert default-constructed GenericValue
    bucket.emplace_back(key, GenericValue());
    ++numEntries;

    // return reference to newly inserted element
    // (the bucket variable is valid because we rehashed earlier if needed)
    for (auto it = bucket.begin(); it != bucket.end(); ++it) {
        if (it->key == key) return it->value;
    }

    // should not reach here
    throw std::logic_error("Failed to return reference after insertion");
}

GenericValue& ChainingHashTable::insert(int64_t key, GenericValue&& value) {
    if (buckets.empty()) {
        buckets.resize(numBuckets);
    }

    // If insertion would exceed load factor threshold, rehash now BEFORE binding any bucket refs
    if ((numEntries + 1) > static_cast<size_t>(numBuckets * LOAD_FACTOR_THRESHOLD)) {
        rehash();
    }

    // Compute index on current buckets
    size_t idx = hashIndex(key, numBuckets);
    auto& bucket = buckets[idx];

    // If key exists, replace value (move) and return
    for (auto it = bucket.begin(); it != bucket.end(); ++it) {
        if (it->key == key) {
            it->value = std::move(value);
            return it->value;
        }
    }

    // Otherwise insert new entry
    bucket.emplace_back(key, std::move(value));
    ++numEntries;

    // Return reference to inserted element (search in same bucket)
    for (auto it = bucket.begin(); it != bucket.end(); ++it) {
        if (it->key == key) return it->value;
    }

    // unreachable
    throw std::logic_error("Failed to find newly inserted element");
}

void ChainingHashTable::erase(int64_t key) {
    if (buckets.empty() || numBuckets == 0) return;

    size_t idx = hashIndex(key, numBuckets);
    auto& bucket = buckets[idx];

    for (auto it = bucket.begin(); it != bucket.end(); ++it) {
        if (it->key == key) {
            bucket.erase(it);
            --numEntries;
            return;
        }
    }
}

// -------------------- iterator --------------------

ChainingHashTable::iterator::iterator() : container(nullptr), bucketIndex(0), entryIt() {}

ChainingHashTable::iterator::iterator(BucketContainer* c, size_t bi, BucketIterator it)
    : container(c), bucketIndex(bi), entryIt(it) {}

void ChainingHashTable::iterator::advance_to_next_valid() {
    if (!container) return;

    // If bucketIndex is within container but entryIt is default, initialize it to begin
    while (bucketIndex < container->size()) {
        if (!container->at(bucketIndex).empty()) {
            if (entryIt == BucketIterator()) {
                entryIt = container->at(bucketIndex).begin();
            }
            if (entryIt != container->at(bucketIndex).end()) {
                return;
            }
        }
        ++bucketIndex;
        entryIt = BucketIterator();
    }

    // mark as end
    entryIt = BucketIterator();
}

ChainingHashTable::iterator& ChainingHashTable::iterator::operator++() {
    if (!container) return *this;
    if (bucketIndex >= container->size()) {
        entryIt = BucketIterator();
        return *this;
    }

    ++entryIt;
    if (entryIt == container->at(bucketIndex).end()) {
        ++bucketIndex;
        entryIt = BucketIterator();
        advance_to_next_valid();
    }
    return *this;
}

ChainingHashTable::iterator ChainingHashTable::iterator::operator++(int) {
    iterator tmp = *this;
    ++(*this);
    return tmp;
}

ChainingHashTable::iterator::pointer ChainingHashTable::iterator::operator->() const {
    return &(*entryIt);
}

ChainingHashTable::iterator::reference ChainingHashTable::iterator::operator*() const {
    return *entryIt;
}

bool ChainingHashTable::iterator::operator==(const iterator& other) const {
    if (container != other.container) return false;
    // both end?
    if ((!container && !other.container) ||
        (container && bucketIndex >= container->size() && other.bucketIndex >= other.container->size())) {
        return true;
    }
    return bucketIndex == other.bucketIndex && entryIt == other.entryIt;
}

bool ChainingHashTable::iterator::operator!=(const iterator& other) const {
    return !(*this == other);
}

ChainingHashTable::iterator ChainingHashTable::begin() {
    if (buckets.empty()) return end();
    size_t idx = 0;
    while (idx < numBuckets && buckets[idx].empty()) ++idx;
    if (idx >= numBuckets) return end();
    return iterator(&buckets, idx, buckets[idx].begin());
}

ChainingHashTable::iterator ChainingHashTable::end() {
    return iterator(&buckets, numBuckets, BucketIterator());
}

ChainingHashTable::iterator ChainingHashTable::find(int64_t key) {
    if (buckets.empty() || numBuckets == 0) return end();
    size_t idx = hashIndex(key, numBuckets);
    for (auto it = buckets[idx].begin(); it != buckets[idx].end(); ++it) {
        if (it->key == key) {
            return iterator(&buckets, idx, it);
        }
    }
    return end();
}

//---------------------------------------------------------------------------

} // namespace hashtable
//---------------------------------------------------------------------------
