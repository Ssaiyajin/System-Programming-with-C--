#ifndef H_lib_MallocAllocator
#define H_lib_MallocAllocator
#pragma once
#include <cstddef>
#include <cstdlib>
#include <new>
#include <type_traits>

namespace pool {

template <typename T>
class MallocAllocator {
public:
    // Standard allocator typedefs
    using value_type = T;
    using pointer = T*;
    using const_pointer = const T*;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;

    // rebind for old-style uses
    template <typename U>
    struct rebind { using other = MallocAllocator<U>; };

    MallocAllocator() noexcept = default;

    template <typename U>
    MallocAllocator(const MallocAllocator<U>&) noexcept {}

    // Preferred allocator interface for allocator_traits
    pointer allocate(size_type n) {
        if (n == 0) return nullptr;
        void* p = std::malloc(n * sizeof(value_type));
        if (!p) throw std::bad_alloc();
        return static_cast<pointer>(p);
    }

    void deallocate(pointer p, size_type /*n*/) noexcept {
        std::free(p);
    }

    // Convenience single-object allocate/deallocate used in tests
    pointer allocate() {
        return allocate(1);
    }

    void deallocate(pointer p) noexcept {
        deallocate(p, 1);
    }

    // Equality required by many allocator concepts
    bool operator==(const MallocAllocator&) const noexcept { return true; }
    bool operator!=(const MallocAllocator& other) const noexcept { return !(*this == other); }
};

} // namespace pool

#endif // H_lib_MallocAllocator
