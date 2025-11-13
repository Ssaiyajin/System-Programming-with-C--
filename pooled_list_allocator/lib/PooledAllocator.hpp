#ifndef H_lib_PooledAllocator
#define H_lib_PooledAllocator
//---------------------------------------------------------------------------
// Minimal, correct pooled allocator implementation used by the tests.
// - allocate() returns a pointer to a default-constructed T
// - deallocate(T*) destroys the object and returns the slot to a free-list
// - Moveable, non-copyable
// - Allocates memory in blocks for contiguous allocations
//---------------------------------------------------------------------------

#include <cstddef>
#include <new>
#include <vector>
#include <utility>
#include <cstdint>

namespace pool {

template <typename T>
class PooledAllocator {
public:
    using value_type = T;

    explicit PooledAllocator(std::size_t block_elems = 1024) noexcept
        : block_size_(block_elems), current_index_(0) {}

    ~PooledAllocator() noexcept {
        for (void* b : blocks_) {
            ::operator delete(b, std::align_val_t(alignof(T)));
        }
    }

    // Move semantics: transfer ownership of blocks and free-list.
    PooledAllocator(PooledAllocator&& other) noexcept {
        steal_from(std::move(other));
    }
    PooledAllocator& operator=(PooledAllocator&& other) noexcept {
        if (this != &other) {
            // free current resources
            for (void* b : blocks_) ::operator delete(b, std::align_val_t(alignof(T)));
            steal_from(std::move(other));
        }
        return *this;
    }

    // Non-copyable
    PooledAllocator(const PooledAllocator&) = delete;
    PooledAllocator& operator=(const PooledAllocator&) = delete;

    // Allocate and default-construct a single T.
    T* allocate() {
        // Reuse freed element if available (LIFO)
        if (!free_list_.empty()) {
            T* p = free_list_.back();
            free_list_.pop_back();
            ::new (static_cast<void*>(p)) T();
            return p;
        }

        // Ensure we have a block with free slots
        if (blocks_.empty() || current_index_ >= block_size_) {
            allocate_block();
        }

        // Compute pointer to next element in last block
        char* base = static_cast<char*>(blocks_.back());
        T* p = reinterpret_cast<T*>(base + current_index_ * sizeof(T));
        ++current_index_;
        ::new (static_cast<void*>(p)) T();
        return p;
    }

    // Destroy and release to free-list
    void deallocate(T* p) noexcept {
        if (!p) return;
        p->~T();
        free_list_.push_back(p);
    }

private:
    std::size_t block_size_;
    std::vector<void*> blocks_;
    std::size_t current_index_ = 0; // index within last block
    std::vector<T*> free_list_;

    void allocate_block() {
        std::size_t bytes = block_size_ * sizeof(T);
        void* mem = ::operator new(bytes, std::align_val_t(alignof(T)));
        blocks_.push_back(mem);
        current_index_ = 0;
    }

    void steal_from(PooledAllocator&& other) noexcept {
        block_size_ = other.block_size_;
        blocks_ = std::move(other.blocks_);
        current_index_ = other.current_index_;
        free_list_ = std::move(other.free_list_);
        other.current_index_ = 0;
        other.block_size_ = 0;
        other.blocks_.clear();
        other.free_list_.clear();
    }
};

} // namespace pool
//--------------------------------------------------------------------------- 
#endif
