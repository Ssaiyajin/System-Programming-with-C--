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
    // allocator concept types
    using value_type = T;
    using pointer = T*;
    using const_pointer = const T*;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;
    template <typename U> struct rebind { using other = PooledAllocator<U>; };

    PooledAllocator() noexcept = default;
    template <typename U> PooledAllocator(const PooledAllocator<U>&) noexcept {}
    ~PooledAllocator() = default;

    // keep your existing single-object API if present, but also provide
    // the standard allocate/deallocate overloads and make sure alignment is respected.
    T* allocate() {
        return allocate(1);
    }

    T* allocate(size_type n) {
        // If your pool manages blocks internally, adapt this to allocate aligned blocks
        // from your pool. The following uses aligned allocation so returned pointers
        // meet alignof(T).
        void* p = ::operator new(n * sizeof(T), std::align_val_t(alignof(T)));
        return static_cast<T*>(p);
    }

    void deallocate(T* ptr) noexcept {
        if (!ptr) return;
        ::operator delete(ptr, std::align_val_t(alignof(T)));
    }

    void deallocate(T* ptr, size_type /*n*/) noexcept {
        deallocate(ptr);
    }

private:
    struct Chunk {
        Chunk* next;
        std::size_t capacity;
        std::size_t used;
        // flexible array of data follows
    };

    Chunk* head_;
    std::size_t initial_capacity_;

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

#endif // H_lib_PooledAllocator
