#ifndef H_lib_List
#define H_lib_List
//---------------------------------------------------------------------------
#include <cstddef>
#include <utility>
#include <concepts>
#include <type_traits>
#include <iterator>
#include <memory>    // allocator_traits, addressof

#include "MallocAllocator.hpp"
#include "PooledAllocator.hpp"

namespace pool {

template<typename T, typename Allocator = MallocAllocator<T>>
class List {
public:
    using value_type = T;
    using allocator_type = Allocator;

private:
    struct Node {
        Node* prev;
        Node* next;
        T value;
    };

    // Use allocator_traits to rebind the allocator to Node robustly
    using NodeAllocator = typename std::allocator_traits<allocator_type>::template rebind_alloc<Node>;
    using NodeAllocTraits = std::allocator_traits<NodeAllocator>;

public:
    List() noexcept(std::is_nothrow_default_constructible_v<NodeAllocator>)
      : head_(nullptr), tail_(nullptr), size_(0), nodeAlloc_() {}

    // copy ctor - requires T copy-constructible
    List(const List& other) requires std::copy_constructible<T>
      : head_(nullptr), tail_(nullptr), size_(0), nodeAlloc_() {
        for (const Node* it = other.head_; it != nullptr; it = it->next) {
            insert(it->value);
        }
    }

    // move ctor
    List(List&& other) noexcept
      : head_(other.head_), tail_(other.tail_), size_(other.size_), nodeAlloc_(std::move(other.nodeAlloc_)) {
        other.head_ = other.tail_ = nullptr;
        other.size_ = 0;
    }

    // copy assign - destroy existing, reset allocator to default, copy
    List& operator=(const List& other) requires std::copy_constructible<T> {
        if (this == &other) return *this;
        clear_nodes();
        nodeAlloc_ = NodeAllocator();
        for (const Node* it = other.head_; it != nullptr; it = it->next) {
            insert(it->value);
        }
        return *this;
    }

    // move assign
    List& operator=(List&& other) noexcept {
        if (this == &other) return *this;
        clear_nodes();
        nodeAlloc_ = std::move(other.nodeAlloc_);
        head_ = other.head_;
        tail_ = other.tail_;
        size_ = other.size_;
        other.head_ = other.tail_ = nullptr;
        other.size_ = 0;
        return *this;
    }

    ~List() { clear_nodes(); }

    std::size_t size() const noexcept { return size_; }

    // insert at end and return reference to inserted value
    T& insert(const T& value) requires std::copy_constructible<T> {
        // allocate raw storage for Node using allocator
        Node* raw = NodeAllocTraits::allocate(nodeAlloc_, 1);
        // placement-new construct Node in allocated memory
        Node* n = raw;
        try {
            ::new (static_cast<void*>(n)) Node{nullptr, nullptr, value};
        } catch (...) {
            NodeAllocTraits::deallocate(nodeAlloc_, raw, 1);
            throw;
        }

        if (!head_) {
            head_ = tail_ = n;
        } else {
            n->prev = tail_;
            tail_->next = n;
            tail_ = n;
        }
        ++size_;
        return n->value;
    }

    // erase first element equal to value
    void erase(const T& value) requires std::equality_comparable<T> {
        for (Node* cur = head_; cur != nullptr; cur = cur->next) {
            if (cur->value == value) {
                if (cur->prev) cur->prev->next = cur->next;
                else head_ = cur->next;

                if (cur->next) cur->next->prev = cur->prev;
                else tail_ = cur->prev;

                cur->~Node();
                NodeAllocTraits::deallocate(nodeAlloc_, cur, 1);
                --size_;
                return;
            }
        }
    }

    // iterator (non-const)
    struct iterator {
        using iterator_category = std::bidirectional_iterator_tag;
        using value_type = T;
        using difference_type = std::ptrdiff_t;
        using pointer = T*;
        using reference = T&;

        Node* node;

        reference operator*() const { return node->value; }
        pointer operator->() const { return std::addressof(node->value); }

        iterator& operator++() { node = node->next; return *this; }
        iterator operator++(int) { iterator tmp = *this; ++*this; return tmp; }

        iterator& operator--() { node = node->prev; return *this; }
        iterator operator--(int) { iterator tmp = *this; --*this; return tmp; }

        bool operator==(const iterator& o) const { return node == o.node; }
        bool operator!=(const iterator& o) const { return node != o.node; }
    };

    struct const_iterator {
        using iterator_category = std::bidirectional_iterator_tag;
        using value_type = T;
        using difference_type = std::ptrdiff_t;
        using pointer = const T*;
        using reference = const T&;

        const Node* node;

        reference operator*() const { return node->value; }
        pointer operator->() const { return std::addressof(node->value); }

        const_iterator& operator++() { node = node->next; return *this; }
        const_iterator operator++(int) { const_iterator tmp = *this; ++*this; return tmp; }

        const_iterator& operator--() { node = node->prev; return *this; }
        const_iterator operator--(int) { const_iterator tmp = *this; --*this; return tmp; }

        bool operator==(const const_iterator& o) const { return node == o.node; }
        bool operator!=(const const_iterator& o) const { return node != o.node; }
    };

    iterator begin() noexcept { return iterator{head_}; }
    iterator end() noexcept { return iterator{nullptr}; }

    const_iterator begin() const noexcept { return const_iterator{head_}; }
    const_iterator end() const noexcept { return const_iterator{nullptr}; }

private:
    Node* head_;
    Node* tail_;
    std::size_t size_;
    NodeAllocator nodeAlloc_;

    void clear_nodes() noexcept {
        Node* cur = head_;
        while (cur) {
            Node* next = cur->next;
            cur->~Node();
            NodeAllocTraits::deallocate(nodeAlloc_, cur, 1);
            cur = next;
        }
        head_ = tail_ = nullptr;
        size_ = 0;
    }
};

} // namespace pool

#endif // POOL_LIST_HPP
// C++
#include "lib/List.hpp"
#include "lib/MallocAllocator.hpp"
#include <gtest/gtest.h>

using namespace pool;
using namespace std;

TEST(TestListExtra, MoveTransferContents) {
    using IntList = List<int, MallocAllocator<int>>;
    IntList l;
    EXPECT_EQ(l.size(), 0u);
    l.insert(1);
    EXPECT_EQ(l.size(), 1u);
    l.insert(2);
    EXPECT_EQ(l.size(), 2u);
    l.insert(3);
    EXPECT_EQ(l.size(), 3u);

    // move-construct
    IntList moved(std::move(l));

    // moved should have the elements, moved-from should be empty
    EXPECT_EQ(moved.size(), 3u);
    EXPECT_EQ(l.size(), 0u);
}

TEST(TestListExtra, MoveAssignmentTransfer) {
    using IntList = List<int, MallocAllocator<int>>;
    IntList a;
    EXPECT_EQ(a.size(), 0u);
    a.insert(42);
    EXPECT_EQ(a.size(), 1u);

    IntList b;
    b = std::move(a);

    EXPECT_EQ(b.size(), 1u);
    EXPECT_EQ(a.size(), 0u);
}

TEST(TestListExtra, MoveDoesNotDuplicate) {
    using IntList = List<int, MallocAllocator<int>>;
    IntList src;
    src.insert(7);
    EXPECT_EQ(src.size(), 1u);

    IntList dst(std::move(src));
    EXPECT_EQ(dst.size(), 1u);
    EXPECT_EQ(src.size(), 0u);

    // Mutate dst and ensure src remains empty
    dst.insert(8);
    EXPECT_EQ(dst.size(), 2u);
    EXPECT_EQ(src.size(), 0u);
}
