#include "lib/ChainingHashTable.hpp"
#include <algorithm>
//---------------------------------------------------------------------------
namespace hashtable {
//---------------------------------------------------------------------------

ChainingHashTable::ChainingHashTable()
    : buckets(numBuckets), numEntries(0) {}

ChainingHashTable::ChainingHashTable(ChainingHashTable&& other) noexcept
    : buckets(std::move(other.buckets)),
      numEntries(other.numEntries),
      numBuckets(other.numBuckets) {
    other.numEntries = 0;
    other.numBuckets = 0;
}

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

size_t ChainingHashTable::hash(int64_t key, size_t size) const {
    return std::hash<int64_t>{}(key) % size;
}

void ChainingHashTable::rehash() {
    size_t newSize = std::max<size_t>(1, numBuckets * 2);
    BucketContainer newBuckets(newSize);

    // Move values into new buckets. Move the GenericValue contents explicitly
    for (auto& bucket : buckets) {
        for (auto& entry : bucket) {
            size_t idx = hash(entry.key, newSize);
            // create a new entry in newBuckets moving the value
            newBuckets[idx].emplace_back(entry.key, std::move(entry.value));
        }
    }

    buckets = std::move(newBuckets);
    numBuckets = newSize;
}

size_t ChainingHashTable::size() const {
    return numEntries;
}

bool ChainingHashTable::contains(int64_t key) const {
    if (buckets.empty()) return false;
    size_t index = hash(key, numBuckets);
    const auto& bucket = buckets[index];
    for (const auto& entry : bucket) {
        if (entry.key == key) return true;
    }
    return false;
}

GenericValue& ChainingHashTable::operator[](int64_t key) {
    if (buckets.empty()) buckets.resize(numBuckets);

    // ensure space before inserting to avoid moving the newly created element
    if (static_cast<float>(numEntries + 1) > static_cast<float>(numBuckets) * LOAD_FACTOR_THRESHOLD) {
        rehash();
    }

    size_t index = hash(key, numBuckets);
    auto& bucket = buckets[index];
    for (auto it = bucket.begin(); it != bucket.end(); ++it) {
        if (it->key == key) return it->value;
    }

    bucket.emplace_back(key, GenericValue());
    ++numEntries;
    return bucket.back().value;
}

GenericValue& ChainingHashTable::insert(int64_t key, GenericValue&& value) {
    if (buckets.empty()) buckets.resize(numBuckets);

    // ensure we have capacity before inserting so element won't be moved after insertion
    if (static_cast<float>(numEntries + 1) > static_cast<float>(numBuckets) * LOAD_FACTOR_THRESHOLD) {
        rehash();
    }

    size_t index = hash(key, numBuckets);
    auto& bucket = buckets[index];

    for (auto it = bucket.begin(); it != bucket.end(); ++it) {
        if (it->key == key) {
            it->value = std::move(value);
            return it->value;
        }
    }

    bucket.emplace_back(key, std::move(value));
    ++numEntries;
    return bucket.back().value;
}

void ChainingHashTable::erase(int64_t key) {
    if (buckets.empty()) return;
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

//--------------------------iterator------------------------------------

ChainingHashTable::iterator::iterator()
    : container(nullptr), bucketIndex(0), entryIt() {}

ChainingHashTable::iterator::iterator(BucketContainer* c, size_t bi, BucketIterator it)
    : container(c), bucketIndex(bi), entryIt(it) {}

bool ChainingHashTable::iterator::is_end() const {
    return container == nullptr || bucketIndex >= container->size();
}

void ChainingHashTable::iterator::advance_to_next_nonempty_bucket() {
    if (!container) return;
    while (bucketIndex < container->size() && container->at(bucketIndex).empty()) {
        ++bucketIndex;
    }
    if (bucketIndex < container->size()) {
        entryIt = container->at(bucketIndex).begin();
    } else {
        entryIt = BucketIterator();
    }
}

ChainingHashTable::iterator ChainingHashTable::begin() {
    if (buckets.empty()) return end();
    size_t idx = 0;
    while (idx < buckets.size() && buckets[idx].empty()) ++idx;
    if (idx >= buckets.size()) return end();
    return iterator(&buckets, idx, buckets[idx].begin());
}

ChainingHashTable::iterator ChainingHashTable::end() {
    return iterator(&buckets, buckets.size(), BucketIterator());
}

ChainingHashTable::iterator ChainingHashTable::find(int64_t key) {
    if (buckets.empty()) return end();
    size_t index = hash(key, numBuckets);
    auto& bucket = buckets[index];
    for (auto it = bucket.begin(); it != bucket.end(); ++it) {
        if (it->key == key) {
            return iterator(&buckets, index, it);
        }
    }
    return end();
}

ChainingHashTable::iterator& ChainingHashTable::iterator::operator++() {
    if (!container) return *this;
    if (bucketIndex >= container->size()) return *this;

    ++entryIt;
    if (entryIt != container->at(bucketIndex).end()) {
        return *this;
    }

    ++bucketIndex;
    while (bucketIndex < container->size() && container->at(bucketIndex).empty()) ++bucketIndex;
    if (bucketIndex < container->size()) {
        entryIt = container->at(bucketIndex).begin();
    } else {
        entryIt = BucketIterator();
    }
    return *this;
}

ChainingHashTable::iterator ChainingHashTable::iterator::operator++(int) {
    iterator tmp = *this;
    ++(*this);
    return tmp;
}

ChainingHashTable::iterator::reference ChainingHashTable::iterator::operator*() const {
    return *entryIt;
}

ChainingHashTable::iterator::pointer ChainingHashTable::iterator::operator->() const {
    return &(*entryIt);
}

bool ChainingHashTable::iterator::operator==(const iterator& other) const {
    if (container != other.container) return false;
    if (is_end() && other.is_end()) return true;
    if (bucketIndex != other.bucketIndex) return false;
    return entryIt == other.entryIt;
}

bool ChainingHashTable::iterator::operator!=(const iterator& other) const {
    return !(*this == other);
}

//---------------------------------------------------------------------------

} // namespace hashtable
//---------------------------------------------------------------------------




