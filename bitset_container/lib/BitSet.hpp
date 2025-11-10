#ifndef H_lib_BitSet
#define H_lib_BitSet
#include <vector>
#include <cstddef>
//---------------------------------------------------------------------------
namespace bitset {
//---------------------------------------------------------------------------
class BitSet {
   private:
    std::vector<bool> bits;

public:
    // Constructors
    BitSet();
    explicit BitSet(size_t size);

    // Member functions
    size_t size() const;
    size_t cardinality() const;
    void push_back(bool value);
    void setFront(bool value);
    void setBack(bool value);

    bool front() const;
    bool back() const;

        class BitReference {
        private:
            size_t pos;
            BitSet* bitset_ptr;

            friend class BitSet;
            BitReference(size_t pos, BitSet* bitset_ptr) : pos(pos), bitset_ptr(bitset_ptr) {}

        public:
            operator bool() const;
            BitReference& operator=(bool value);
            BitReference& operator=(const BitReference& other);
        };

        // Subscript operator overloads
        bool operator[](size_t pos) const; 
        BitReference operator[](size_t pos);

        // Add more functions here for iterators, range-for support, etc.
        // Nested BitIterator class
        class BitIterator {
        public:
            using difference_type = std::ptrdiff_t;
            using value_type = bool;
            using reference = BitReference;
            using iterator_category = std::bidirectional_iterator_tag;

        private:
            size_t index;
            BitSet* bitset_ptr;

        public:
            // Constructors
            BitIterator();
            BitIterator(size_t index, BitSet* bitset_ptr);

            // Operators
            bool operator==(const BitIterator& other) const;
            bool operator!=(const BitIterator& other) const;
            BitReference operator*() const;
            BitIterator& operator++(); // Pre-increment
            BitIterator operator++(int); // Post-increment
            BitIterator& operator--(); // Pre-decrement
            BitIterator operator--(int); // Post-decrement
        };

        // Functions to obtain iterators
        BitIterator begin();
        BitIterator end();
    
    // Nested ConstBitIterator class
        class ConstBitIterator {
        public:
            using difference_type = std::ptrdiff_t;
            using value_type = bool;
            using reference = bool; // Alias for bool as per requirement
            using iterator_category = std::bidirectional_iterator_tag;

        private:
            size_t index;
            const BitSet* bitset_ptr;

        public:
            // Constructors
            //ConstBitIterator();
            ConstBitIterator(size_t index, const BitSet* bitset_ptr);

            // Operators
            bool operator==(const ConstBitIterator& other) const;
            bool operator!=(const ConstBitIterator& other) const;
            bool operator*() const;
            ConstBitIterator& operator++(); // Pre-increment
            ConstBitIterator operator++(int); // Post-increment
            ConstBitIterator& operator--(); // Pre-decrement
            ConstBitIterator operator--(int); // Post-decrement
        };

        // Functions to obtain const iterators
        ConstBitIterator begin() const;
        ConstBitIterator end() const;
    
};
//---------------------------------------------------------------------------
} // namespace bitset
//---------------------------------------------------------------------------
#endif
//---------------------------------------------------------------------------
