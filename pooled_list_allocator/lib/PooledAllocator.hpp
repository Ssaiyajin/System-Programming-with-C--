#pragma once

#include <cstddef>
#include <cstdint>
#include <vector>
#include <new>
#include <algorithm>
#include <type_traits>

namespace pool {

template <typename T>
class PooledAllocator {
public:
    // Allocator concept types
    using value_type = T;
    using pointer = T*;
    using const_pointer = const T*;
    using reference = T&;
    using const_reference = const T&;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;

    // rebind helper (C++11+)
    template <typename U>
    using rebind = PooledAllocator<U>;

    // propagation / equality traits (help allocator-aware containers)
    using propagate_on_container_move_assignment = std::true_type;
    using is_always_equal = std::false_type;

    // constructors / assignment
    PooledAllocator() noexcept = default;
    PooledAllocator(const PooledAllocator&) noexcept = default;
    PooledAllocator& operator=(const PooledAllocator&) noexcept = default;

    template <typename U>
    PooledAllocator(const PooledAllocator<U>&) noexcept {}

    PooledAllocator(PooledAllocator&& other) noexcept
        : blocks_(std::move(other.blocks_)),
          free_list_(std::move(other.free_list_)),
          current_(other.current_),
          block_end_(other.block_end_)
    {
        other.current_ = other.block_end_ = nullptr;
    }

    PooledAllocator& operator=(PooledAllocator&& other) noexcept {
        if (this == &other) return *this;
        clear_blocks();
        blocks_ = std::move(other.blocks_);
        free_list_ = std::move(other.free_list_);
        current_ = other.current_;
        block_end_ = other.block_end_;
        other.current_ = other.block_end_ = nullptr;
        return *this;
    }

    ~PooledAllocator() noexcept {
        clear_blocks();
    }

    // allocate overloads: tests call a.allocate() and allocator_traits expects allocate(size_type)
    pointer allocate() {
        return allocate(1u);
    }

    pointer allocate(size_type n) {
        if (n == 0) return nullptr;

        // reuse single-object free slots first
        if (n == 1 && !free_list_.empty()) {
            pointer p = free_list_.back();
            free_list_.pop_back();
            return p;
        }

        if (current_ == nullptr || static_cast<size_type>(block_end_ - current_) < n) {
            allocate_block(std::max(n, ITEMS_PER_BLOCK));
        }

        pointer result = current_;
        current_ += n;
        return result;
    }

    // deallocate overloads: tests call a.deallocate(ptr)
    void deallocate(pointer p) noexcept {
        deallocate(p, 1u);
    }

    void deallocate(pointer p, size_type n) noexcept {
        if (!p) return;
        if (n == 1) free_list_.push_back(p);
        // for n > 1 we don't do per-range bookkeeping — blocks freed in destructor
    }

    template <typename U>
    bool operator==(const PooledAllocator<U>&) const noexcept { return true; }

    template <typename U>
    bool operator!=(const PooledAllocator<U>&) const noexcept { return false; }

private:
    static constexpr size_type ITEMS_PER_BLOCK = 1024;

    void allocate_block(size_type items) {
        const size_type bytes = items * sizeof(T);
        void* mem = ::operator new(bytes, std::align_val_t(alignof(T)));
        blocks_.push_back(mem);
        current_ = static_cast<T*>(mem);
        block_end_ = current_ + items;
    }

    void clear_blocks() noexcept {
        for (void* b : blocks_) {
            ::operator delete(b, std::align_val_t(alignof(T)));
        }
        blocks_.clear();
        free_list_.clear();
        current_ = block_end_ = nullptr;
    }

    std::vector<void*> blocks_;
    std::vector<pointer> free_list_;
    pointer current_ = nullptr;
    pointer block_end_ = nullptr;
};

} // namespace pool
