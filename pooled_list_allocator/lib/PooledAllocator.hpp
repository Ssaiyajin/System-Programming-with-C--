#ifndef H_lib_PooledAllocator
#define H_lib_PooledAllocator

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
    using pointer = T*;
    using const_pointer = const T*;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;

    template<typename U>
    struct rebind { using other = PooledAllocator<U>; };

    PooledAllocator() noexcept : head_(nullptr), initial_capacity_(8) {}
    
    // Disable copy; enable move
    PooledAllocator(const PooledAllocator&) = delete;
    PooledAllocator& operator=(const PooledAllocator&) = delete;

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

    ~PooledAllocator() noexcept {
        release_all();
    }

    // ===== STL-compliant interface =====
    T* allocate(size_type n) {
        if (n == 0) return nullptr;
        if (n != 1) {
            // optional: only support single-object allocations for simplicity
            throw std::bad_alloc();
        }
        return allocate_single();
    }

    void deallocate(pointer p, size_type n) noexcept {
        if (n == 1) deallocate_single(p);
    }

    bool operator==(const PooledAllocator&) const noexcept { return true; }
    bool operator!=(const PooledAllocator&) const noexcept { return false; }

private:
    struct Chunk {
        Chunk* next;
        std::size_t capacity;
        std::size_t used;
    };

    Chunk* head_;
    std::size_t initial_capacity_;

    T* allocate_single() {
        if (!head_ || head_->used >= head_->capacity) {
            allocate_chunk(next_capacity());
        }
        unsigned char* data = reinterpret_cast<unsigned char*>(head_ + 1);
        T* result = reinterpret_cast<T*>(data + head_->used * sizeof(T));
        head_->used += 1;
        return result;
    }

    void deallocate_single(T* p) noexcept {
        if (!p || !head_ || head_->used == 0) return;
        unsigned char* data = reinterpret_cast<unsigned char*>(head_ + 1);
        unsigned char* last_addr = data + (head_->used - 1) * sizeof(T);
        if (reinterpret_cast<unsigned char*>(p) == last_addr) {
            head_->used -= 1; // reclaim last slot
        }
    }

    std::size_t next_capacity() const noexcept {
        return head_ ? head_->capacity * 2 : initial_capacity_;
    }

    void allocate_chunk(std::size_t capacity) {
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
            std::free(cur);
            cur = next;
        }
        head_ = nullptr;
    }
};

} // namespace pool

#endif // H_lib_PooledAllocator
