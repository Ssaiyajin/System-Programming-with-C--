#ifndef H_lib_List
#define H_lib_List
//---------------------------------------------------------------------------
#include <cstddef>
#include <utility>
#include <concepts>
#include <type_traits>
#include <iterator>
#include <memory>    // addressof
#include <cassert>

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

    using NodeAllocator = typename allocator_type::template rebind<Node>::other;

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
        Node* raw = nodeAlloc_.allocate();
        Node* n = reinterpret_cast<Node*>(raw);
        // placement-new construct Node in allocated memory
        try {
            new (n) Node{nullptr, nullptr, value};
        } catch (...) {
            nodeAlloc_.deallocate(raw);
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

                // explicitly call destructor and return memory to allocator
                cur->~Node();
                nodeAlloc_.deallocate(cur);
                --size_;
                return;
            }
        }
    }

    // iterator
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
            nodeAlloc_.deallocate(cur);
            cur = next;
        }
        head_ = tail_ = nullptr;
        size_ = 0;
    }
};

} // namespace pool

#endif // POOL_LIST_HPP
