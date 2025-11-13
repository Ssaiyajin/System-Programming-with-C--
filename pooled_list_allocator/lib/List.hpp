#ifndef H_lib_List
#define H_lib_List
//---------------------------------------------------------------------------
#include <cstddef>
#include <utility>
#include <concepts>
#include <type_traits>
#include <cassert>

template<typename T, typename Allocator = MallocAllocator<T>>
class List {
public:
    using value_type = T;
    using allocator_type = Allocator;
private:
    struct Node {
        Node* prev;
        Node* next;
        // value stored in-place
        T value;
    };

    // Allocator for Node objects (rebind)
    using NodeAllocator = typename allocator_type::template rebind<Node>::other;

public:
    List() noexcept(std::is_nothrow_default_constructible_v<NodeAllocator>)
      : head_(nullptr), tail_(nullptr), size_(0), nodeAlloc_() {}

    // Copy constructor - requires T to be copy-constructible
    List(const List& other) requires std::copy_constructible<T>
      : head_(nullptr), tail_(nullptr), size_(0), nodeAlloc_()
    {
        for (Node* cur = other.head_; cur != nullptr; cur = cur->next) {
            insert(cur->value);
        }
    }

    // Move constructor - move resources and allocator
    List(List&& other) noexcept
      : head_(other.head_), tail_(other.tail_), size_(other.size_), nodeAlloc_(std::move(other.nodeAlloc_))
    {
        other.head_ = other.tail_ = nullptr;
        other.size_ = 0;
    }

    // Copy assignment - remove elements, reset allocator to default, then copy elements
    List& operator=(const List& other) requires std::copy_constructible<T> {
        if (this == &other) return *this;
        clear_nodes();
        nodeAlloc_ = NodeAllocator(); // default-constructed allocator
        for (Node* cur = other.head_; cur != nullptr; cur = cur->next) {
            insert(cur->value);
        }
        return *this;
    }

    // Move assignment - remove current elements, move resources from other
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

    ~List() {
        clear_nodes();
    }

    std::size_t size() const noexcept { return size_; }

    // insert at end, return reference to inserted value
    T& insert(const T& value) requires std::copy_constructible<T> {
        Node* raw = nodeAlloc_.allocate(); // alloc raw storage for Node
        // placement-new construct Node in place: need to set prev/next then construct value
        Node* n;
        try {
            n = ::new (raw) Node{nullptr, nullptr, value}; // uses T copy-constructor
        } catch (...) {
            // if construction fails, we should deallocate the raw memory
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

    // erase first element equal to value; requires equality comparable
    void erase(const T& value) requires std::equality_comparable<T> {
        for (Node* cur = head_; cur != nullptr; cur = cur->next) {
            if (cur->value == value) {
                // remove cur
                if (cur->prev) cur->prev->next = cur->next;
                else head_ = cur->next;

                if (cur->next) cur->next->prev = cur->prev;
                else tail_ = cur->prev;

                // call destructor of Node (which will destroy T)
                cur->~Node();
                nodeAlloc_.deallocate(cur);
                --size_;
                return;
            }
        }
    }

    // Iteration support (non-const)
    struct iterator {
        Node* node;
        using iterator_category = std::bidirectional_iterator_tag;
        using value_type = T;
        using difference_type = std::ptrdiff_t;
        using pointer = T*;
        using reference = T&;

        reference operator*() const { return node->value; }
        pointer operator->() const { return std::addressof(node->value); }

        iterator& operator++() { node = node->next; return *this; }
        iterator operator++(int) { iterator tmp = *this; ++*this; return tmp; }

        iterator& operator--() { node = node->prev; return *this; }
        iterator operator--(int) { iterator tmp = *this; --*this; return tmp; }

        bool operator==(const iterator& o) const { return node == o.node; }
        bool operator!=(const iterator& o) const { return node != o.node; }
    };

    iterator begin() noexcept { return iterator{head_}; }
    iterator end() noexcept { return iterator{nullptr}; }

    // disable const-iteration for brevity; can be added similarly if desired.

private:
    Node* head_;
    Node* tail_;
    std::size_t size_;
    NodeAllocator nodeAlloc_;

    void clear_nodes() noexcept {
        Node* cur = head_;
        while (cur) {
            Node* next = cur->next;
            // destroy
            cur->~Node();
            nodeAlloc_.deallocate(cur);
            cur = next;
        }
        head_ = tail_ = nullptr;
        size_ = 0;
    }
};

#endif // LIST_HPP
