#ifndef H_lib_PooledAllocator
#define H_lib_PooledAllocator
//---------------------------------------------------------------------------
// Minimal pooled allocator implementation used by the tests.
// - allocate() default-constructs one T and returns T*
// - deallocate(T*) destroys the object and returns the slot to a free-list
// - Moveable, non-copyable
// - Allocates memory in blocks for contiguous allocations
//---------------------------------------------------------------------------

#include <cstdlib>
#include <cstddef>
#include <new>
#include <utility>
#include <type_traits>
#include <cassert>

namespace pool {

template<typename T>
class PooledAllocator {
public:
    using value_type = T;

    PooledAllocator() noexcept
      : head_(nullptr), initial_capacity_(8) {}

    // disallow copy (cheap move only)
    PooledAllocator(const PooledAllocator&) = delete;
    PooledAllocator& operator=(const PooledAllocator&) = delete;

    // move
    PooledAllocator(PooledAllocator&& other) noexcept
      : head_(other.head_), initial_capacity_(other.initial_capacity_) {
        other.head_ = nullptr;
    }

    PooledAllocator& operator=(PooledAllocator&& other) noexcept {
        if (this != &other) {
            release_all();
            head_ = other.head_;
            initial_capacity_ = other.initial_capacity_;
            other.head_ = nullptr;
        }
        return *this;
    }

    template<typename U>
    struct rebind { using other = PooledAllocator<U>; };

    ~PooledAllocator() noexcept {
        release_all();
    }

    // allocate storage for one T (uninitialized)
    T* allocate() {
        if (!head_ || head_->used >= head_->capacity) {
            allocate_chunk(next_capacity());
        }
        unsigned char* data = reinterpret_cast<unsigned char*>(head_ + 1);
        T* result = reinterpret_cast<T*>(data + head_->used * sizeof(T));
        head_->used += 1;
        return result;
    }

    // deallocate only if this pointer is the last allocated in the most recent chunk
    void deallocate(T* p) noexcept {
        if (!p || !head_) return;
        unsigned char* data = reinterpret_cast<unsigned char*>(head_ + 1);
        if (head_->used == 0) return;
        unsigned char* last_addr = data + (head_->used - 1) * sizeof(T);
        if (reinterpret_cast<unsigned char*>(p) == last_addr) {
            // pop
            head_->used -= 1;
        }
    }

private:
    struct Chunk {
        Chunk* next;
        std::size_t capacity;
        std::size_t used;
        // data follows
    };

    Chunk* head_;
    std::size_t initial_capacity_;

    std::size_t next_capacity() const noexcept {
        return head_ ? (head_->capacity * 2) : initial_capacity_;
    }

    void allocate_chunk(std::size_t capacity) {
        // allocate: sizeof(Chunk) + capacity * sizeof(T)
        std::size_t bytes = sizeof(Chunk) + capacity * sizeof(T);
        void* mem = std::malloc(bytes);
        if (!mem) throw std::bad_alloc();
        Chunk* ch = reinterpret_cast<Chunk*>(mem);
        ch->next = head_;
        ch->capacity = capacity;
        ch->used = 0;
        head_ = ch;
    }

    void release_all() noexcept {
        Chunk* cur = head_;
        while (cur) {
            Chunk* next = cur->next;
            std::free(reinterpret_cast<void*>(cur));
            cur = next;
        }
        head_ = nullptr;
    }
};

} // namespace pool

#endif