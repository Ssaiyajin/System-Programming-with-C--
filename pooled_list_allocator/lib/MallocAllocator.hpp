#ifndef H_lib_MallocAllocator
#define H_lib_MallocAllocator
#pragma once
#include <cstdlib>
#include <new>
#include <type_traits>
#include <cstddef>

namespace pool {

template <typename T>
class MallocAllocator {
public:
    using value_type = T;
    using pointer = T*;
    using const_pointer = const T*;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;

    template <class U>
    struct rebind { using other = MallocAllocator<U>; };

    MallocAllocator() noexcept = default;

    template <class U>
    MallocAllocator(const MallocAllocator<U>&) noexcept {}

    // Standard allocator interface (n objects)
    pointer allocate(size_type n) {
        if (n == 0) return nullptr;
        if (n > (size_type(-1) / sizeof(T))) throw std::bad_alloc();
        void* p = std::malloc(n * sizeof(T));
        if (!p) throw std::bad_alloc();
        return static_cast<pointer>(p);
    }

    void deallocate(pointer p, size_type) noexcept {
        std::free(static_cast<void*>(p));
    }

    // Test-friendly overloads expected by the unit tests:
    pointer allocate() {
        return allocate(1);
    }

    void deallocate(pointer p) noexcept {
        deallocate(p, 1);
    }

    // Comparisons
    bool operator==(const MallocAllocator&) const noexcept { return true; }
    bool operator!=(const MallocAllocator&) const noexcept { return false; }
};

} // namespace pool