#ifndef H_lib_MallocAllocator
#define H_lib_MallocAllocator

#include <cstdlib>
#include <cstddef>
#include <new>
#include <type_traits>

namespace pool {

template <typename T>
class MallocAllocator {
public:
    using value_type = T;

    // Mandatory typedefs
    using pointer = T*;
    using const_pointer = const T*;
    using reference = T&;
    using const_reference = const T&;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;

    // Required by STL allocator model
    template <typename U>
    struct rebind {
        using other = MallocAllocator<U>;
    };

    MallocAllocator() noexcept {}
    template <typename U>
    MallocAllocator(const MallocAllocator<U>&) noexcept {}

    pointer allocate(size_type n) {
        if (n == 0)
            return nullptr;

        if (n > static_cast<size_type>(-1) / sizeof(T))
            throw std::bad_alloc();

        void* p = std::malloc(n * sizeof(T));
        if (!p)
            throw std::bad_alloc();
        return static_cast<pointer>(p);
    }

    void deallocate(pointer p, size_type) noexcept {
        std::free(p);
    }
};
} // namespace pool

#endif