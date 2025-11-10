#ifndef H_lib_MultiMap
#define H_lib_MultiMap
//---------------------------------------------------------------------------
#include <cstdint>
#include <utility>
#include <vector>
//---------------------------------------------------------------------------
namespace hashtable {
//---------------------------------------------------------------------------
/// A multimap based on a chaining hash table
class MultiMap {
    public:
    /// Value type
    using value_type = std::pair<uint32_t, uint32_t>;

    private:
    /// A hash table node
    struct Node {
        /// The value of the node
        value_type value = {};
        /// The next node in the chain
        Node* next = nullptr;

        /// Constructor
        Node() = default;
        /// Constructor
        explicit Node(value_type value);
    };

    /// The hash table
    std::vector<Node*> hashTable;
    /// The node allocator
    std::allocator<Node> nodeAllocator;

    public:
    /// An iterator
    class Iterator {
        public:
        /// The difference between two iterators
        using difference_type = std::ptrdiff_t;
        /// The type obtained when dereferencing an iterator
        using value_type = MultiMap::value_type;
        /// A pointer to the value type
        using pointer = value_type*;
        /// A reference to the value type
        using reference = value_type&;
        /// The iterator category
        using iterator_category = std::forward_iterator_tag;

        private:
        /// The current bucket
        Node** bucket;
        /// The end of the bucket vector
        Node** end;
        /// The current node
        Node* node;

        /// Advance to the next entry
        void advance();

        friend class MultiMap;

        /// Constructor
        Iterator(Node** bucket, Node** end, Node* node);

        public:
        /// Dereference
        reference operator*();
        /// Pointer to member
        pointer operator->();

        /// Pre-increment
        Iterator& operator++();
        /// Post-increment
        Iterator operator++(int);

        /// Equality comparison
        bool operator==(const Iterator& other) const;
    };

    /// Constructor
    explicit MultiMap(uint32_t bucketCount);
    /// Destructor
    ~MultiMap();

    /// Insert an entry
    Iterator insert(value_type value);
    /// Find the first entry with the specified key
    Iterator findFirst(uint32_t key);
    /// Find the next entry with the specified key (if any)
    Iterator findNext(Iterator& it);

    /// Return an iterator pointing to the first element
    Iterator begin();
    /// Return an iterator pointing past the last element
    Iterator end();
};
//---------------------------------------------------------------------------
} // namespace hashtable
//---------------------------------------------------------------------------
#endif
//---------------------------------------------------------------------------
