#include "lib/BitSet.hpp"
//---------------------------------------------------------------------------
namespace bitset {
//---------------------------------------------------------------------------
// Constructors
BitSet::BitSet() : bits() {}

BitSet::BitSet(size_t size) : bits(size) {}

// Member functions
size_t BitSet::size() const {
    return bits.size();
}

size_t BitSet::cardinality() const {
    size_t count = 0;
    for (bool bit : bits) {
        if (bit) {
            count++;
        }
    }
    return count;
}

void BitSet::push_back(bool value) {
    bits.push_back(value);
}

void BitSet::setFront(bool value){
    if (!bits.empty()) {
        bits.front() = value;
    }
}

void BitSet::setBack(bool value) {
    if (!bits.empty()) {
        bits.back() = value;
    }
}

bool BitSet::front() const {
    if (!bits.empty()) {
        return bits.front();
    }
    return false; // Adjust the default return value as needed
}

bool BitSet::back() const {
    if (!bits.empty()) {
        return bits.back();
    }
    return false; // Adjust the default return value as needed
}

bool BitSet::operator[](size_t pos) const {
    return bits[pos];
}

BitSet::BitReference BitSet::operator[](size_t pos) {
    return BitReference(pos, this);
}
// BitReference class methods
bitset::BitSet::BitReference::operator bool() const {
    return bitset_ptr->bits[pos];
}

BitSet::BitReference& BitSet::BitReference::operator=(bool value) {
    bitset_ptr->bits[pos] = value;
    return *this;
}

BitSet::BitReference& BitSet::BitReference::operator=(const BitReference& other) {
    bitset_ptr->bits[pos] = other.bitset_ptr->bits[other.pos];
    return *this;
}
// BitIterator class implementation
BitSet::BitIterator::BitIterator() : index(0), bitset_ptr(nullptr) {}

    BitSet::BitIterator::BitIterator(size_t index, BitSet* bitset_ptr) : index(index), bitset_ptr(bitset_ptr) {}

    bool BitSet::BitIterator::operator==(const BitIterator& other) const {
        return (index == other.index) && (bitset_ptr == other.bitset_ptr);
    }

    bool BitSet::BitIterator::operator!=(const BitIterator& other) const {
        return !(*this == other);
    }

    BitSet::BitReference BitSet::BitIterator::operator*() const {
        return (*bitset_ptr)[index];
    }

    BitSet::BitIterator& BitSet::BitIterator::operator++() {
        ++index;
        return *this;
    }

    BitSet::BitIterator BitSet::BitIterator::operator++(int) {
        BitIterator temp = *this;
        ++index;
        return temp;
    }

    BitSet::BitIterator& BitSet::BitIterator::operator--() {
        --index;
        return *this;
    }

    BitSet::BitIterator BitSet::BitIterator::operator--(int) {
        BitIterator temp = *this;
        --index;
        return temp;
    }

   BitSet::BitIterator BitSet::begin() {
    return BitIterator(0, this);
    }

    
    BitSet::BitIterator BitSet::end() {
    return BitIterator(bits.size(), this);
    }

// ConstBitIterator class implementation
//BitSet::ConstBitIterator::ConstBitIterator() : index(0), bitset_ptr(nullptr) {}

BitSet::ConstBitIterator::ConstBitIterator(size_t index, const BitSet* bitset_ptr)
        : index(index), bitset_ptr(bitset_ptr) {}

bool BitSet::ConstBitIterator::operator==(const ConstBitIterator& other) const {
    return (index == other.index) && (bitset_ptr == other.bitset_ptr);
}

bool BitSet::ConstBitIterator::operator!=(const ConstBitIterator& other) const {
    return !(*this == other);
}

bool BitSet::ConstBitIterator::operator*() const {
    return (*bitset_ptr)[index];
}

BitSet::ConstBitIterator& BitSet::ConstBitIterator::operator++() {
    ++index;
    return *this;
}

BitSet::ConstBitIterator BitSet::ConstBitIterator::operator++(int) {
    ConstBitIterator temp = *this;
    ++index;
    return temp;
}

BitSet::ConstBitIterator& BitSet::ConstBitIterator::operator--() {
    --index;
    return *this;
}

BitSet::ConstBitIterator BitSet::ConstBitIterator::operator--(int) {
    ConstBitIterator temp = *this;
    --index;
    return temp;
}

// begin() method for const BitSet
    BitSet::ConstBitIterator BitSet::begin() const {
        return ConstBitIterator(0, this); // Assuming the starting index is 0
    }

    // end() method for const BitSet
    BitSet::ConstBitIterator BitSet::end() const {
        return ConstBitIterator(size(), this); // End index might be size()
    }
//---------------------------------------------------------------------------
} // namespace bitset
//---------------------------------------------------------------------------
