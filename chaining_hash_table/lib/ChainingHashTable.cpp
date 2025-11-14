#include "lib/ChainingHashTable.hpp"
#include <cmath>
#include <cstring>
#include <memory>
//---------------------------------------------------------------------------
namespace hashtable {
//---------------------------------------------------------------------------
// Constructor
ChainingHashTable::ChainingHashTable() : numEntries(0) {}
// Move constructor
ChainingHashTable::ChainingHashTable(ChainingHashTable&& other) noexcept
    : buckets(std::move(other.buckets)),
      numEntries(other.numEntries) {
    other.numBuckets = 0;
}


ChainingHashTable::~ChainingHashTable() {
    // Clear each list in the bucket
    for (auto& entryList : buckets) {
        entryList.clear();  // Or use smart pointers to automatically manage memory
    }
    buckets.clear();  // Clear the entire container
}

ChainingHashTable& ChainingHashTable::operator=(ChainingHashTable&& other) noexcept {
    if (this != &other) {
        // Release current resources
        buckets.clear();

        // Move resources from 'other'
        buckets = std::move(other.buckets);
        numEntries = other.numEntries;
        numBuckets = other.numBuckets;

        // Reset 'other'
        other.numEntries = 0;
        other.numBuckets = 0;
    }
    return *this;
}
void ChainingHashTable::rehash() {
    size_t newSize = numBuckets * 2;
    BucketContainer newBuckets(newSize);

    for (auto& bucketList : buckets) {
        auto entryIt = bucketList.begin();
        while (entryIt != bucketList.end()) {
            Entry entry = std::move(*entryIt);
            entryIt = bucketList.erase(entryIt);
            size_t index = hash(entry.key, newSize);
            newBuckets[index].push_back(std::move(entry));
        }
    }

    // Clear the old buckets after moving elements
    buckets.clear();

    // Assign the new buckets to the current object
    buckets = std::move(newBuckets);
}

size_t ChainingHashTable::hash(int64_t key, size_t size) const {
    return std::hash<int64_t>{}(key) % size;
}

size_t ChainingHashTable::size() const {
    return numEntries;
}

bool ChainingHashTable::contains(int64_t key) const {
    size_t index = hash(key, numBuckets);
    auto& bucket = buckets[index];
    auto entry = bucket.begin();
    while (entry != bucket.end())
    {
        if (entry->key == key)
        {
            return true;
        }
        ++entry;
    }
    return false;
}

GenericValue& ChainingHashTable::operator[](int64_t key) {
    if (buckets.empty()) {
        buckets.resize(numBuckets);
    }
    size_t index = hash(key, numBuckets);
    auto& bucket = buckets[index];
    auto entry = bucket.begin();
    while (!bucket.empty())
    {
        if (entry->key == key)
        {
            return entry->value;
        }
        ++entry; // Move to the next entry in the bucket
    }
    return insert(key, GenericValue()); // Create a new entry with a default value
}


GenericValue& ChainingHashTable::insert(int64_t key, GenericValue&& value) {
    if (buckets.empty()) {
        buckets.resize(numBuckets);
    }

    size_t index = hash(key, numBuckets);
    auto& bucket = buckets[index];

    // Use auto for iterator type
    for (auto entryIt = bucket.begin(); entryIt != bucket.end(); ++entryIt) {
        if (entryIt->key == key) {
            return entryIt->value;
        }
    }

    // Use emplace_back to construct the Entry in-place
    bucket.emplace_back(key, std::move(value));

    numEntries++;

    if (numEntries > numBuckets * LOAD_FACTOR_THRESHOLD) {
        rehash();
    }

    // Return a reference to the newly inserted value
    return bucket.back().value;
}

void ChainingHashTable::erase(int64_t key) {
    size_t index = hash(key, numBuckets);
    auto& bucket = buckets[index];

    auto entry = bucket.begin();
    Entry* prev = nullptr;

    while (entry != bucket.end()) {
        if (entry->key == key) {
            if (prev != nullptr) {
                if (std::next(entry) != bucket.end()) {
                    // Move data instead of pointer
                    prev->value = std::move(entry->value);
                }
                entry = bucket.erase(entry);  // Update iterator after erasing
            } else {
                if (!bucket.empty()) {
                    // If prev is null, we are at the first element
                    if (std::next(entry) != bucket.end()) {
                        // Move data instead of pointer
                        std::next(entry)->value = std::move(entry->value);
                    }
                    entry = bucket.erase(entry);  // Update iterator after erasing
                } else {
                    // Handle the case where the bucket is empty
                    --numEntries;
                    return;
                }
            }

            --numEntries;
            return;
        }
        prev = &(*entry);
        ++entry;
    }
}



//--------------------------iterator------------------------------------


ChainingHashTable::iterator::iterator()
    : container(nullptr), bucketIndex(0), entryIterator(nullptr) {}

ChainingHashTable::iterator::iterator(BucketContainer* c, size_t bi, Entry* it)
    : container(c), bucketIndex(bi), entryIterator(it) {}


ChainingHashTable::iterator ChainingHashTable::begin() {
    size_t index = 0;
    while (index < numBuckets && buckets[index].empty()) {
        ++index;
    }
    return iterator(&buckets, index, (index < numBuckets) ? &buckets[index].front() : nullptr);
}

ChainingHashTable::iterator ChainingHashTable::end() {
    return iterator(&buckets, numBuckets, nullptr);
}

ChainingHashTable::iterator ChainingHashTable::find(int64_t key) {
    if (buckets.empty()) {
        return end();
    }

    size_t index = hash(key, numBuckets);
    for (Entry& entry : buckets[index]) {
        if (entry.key == key) {
            return iterator(&buckets, index, &entry);
        }
    }

    return end();
}
ChainingHashTable::iterator& ChainingHashTable::iterator::operator++() {
    if (entryIterator) {
        entryIterator = entryIterator->test.get();
    }
    if (!entryIterator) {
        next();
    }
    return *this;
}

void ChainingHashTable::iterator::next() {
    while (bucketIndex < container->size() && container->at(bucketIndex).empty()) {
        ++bucketIndex;
    }

    if (bucketIndex < container->size()) {
        entryIterator = &container->at(bucketIndex).front();

        // Move to the next entry in the bucket
        if (entryIterator) {
            entryIterator = entryIterator->test.get();
        }

        // Skip empty buckets
        while (!entryIterator && bucketIndex < container->size() - 1) {
            ++bucketIndex;
            if (bucketIndex < container->size()) {
                entryIterator = &container->at(bucketIndex).front();
            }
        }
    } else {
        // End of container
        entryIterator = nullptr;
    }
}

ChainingHashTable::iterator ChainingHashTable::iterator::operator++(int) {
    iterator temp = *this;
    ++(*this);
    return temp;
}
ChainingHashTable::iterator::reference ChainingHashTable::iterator::operator*() const {
    return *entryIterator;
}

ChainingHashTable::iterator::pointer ChainingHashTable::iterator::operator->() const {
    return entryIterator;
} 
bool ChainingHashTable::iterator::operator==(const iterator& other) const {
    return container == other.container && bucketIndex == other.bucketIndex && entryIterator == other.entryIterator;
   }

bool ChainingHashTable::iterator::operator!=(const iterator& other) const {
    return !(*this == other);
}
//---------------------------------------------------------------------------
} // namespace hashtable
//---------------------------------------------------------------------------




