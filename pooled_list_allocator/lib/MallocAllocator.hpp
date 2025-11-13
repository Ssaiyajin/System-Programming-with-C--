#ifndef H_lib_MallocAllocator
#define H_lib_MallocAllocator

#include <cstdlib>
#include <cstddef>
#include <new>
#include <type_traits>

namespace pool {

template<typename T>
struct MallocAllocator {
    using value_type = T;
    using pointer = T*;
    using const_pointer = const T*;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;

    MallocAllocator() noexcept = default;

    // Converting constructor so allocators of different T are interconvertible
    template<typename U>
    MallocAllocator(const MallocAllocator<U>&) noexcept {}

    MallocAllocator(const MallocAllocator&) noexcept = default;
    MallocAllocator(MallocAllocator&&) noexcept = default;
    MallocAllocator& operator=(const MallocAllocator&) noexcept = default;
    MallocAllocator& operator=(MallocAllocator&&) noexcept = default;

    template<typename U>
    struct rebind { using other = MallocAllocator<U>; };

    // Allocate storage for one T (uninitialized)
    T* allocate() {
        void* p = std::malloc(sizeof(T));
        if (!p) throw std::bad_alloc();
        return static_cast<T*>(p);
    }

    // Deallocate storage previously returned by allocate()
    void deallocate(T* p) noexcept {
        std::free(static_cast<void*>(p));
    }
};

} // namespace pool

#endif // POOL_MALLOC_ALLOCATOR_HPP