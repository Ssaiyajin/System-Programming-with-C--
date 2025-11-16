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
    // Standard allocator typedefs required by trait checks / allocator_traits
    using value_type = T;
    using pointer = T*;
    using const_pointer = const T*;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;

    template <typename U>
    struct rebind { using other = MallocAllocator<U>; };

    MallocAllocator() noexcept = default;

    // Allow construction from other allocator types
    template <typename U>
    MallocAllocator(const MallocAllocator<U>&) noexcept {}

    // allocate / deallocate with single-object semantics (used by tests)
    T* allocate() {
        return static_cast<T*>(std::malloc(sizeof(T)));
    }

    void deallocate(T* p) {
        std::free(p);
    }

    // allocator_traits-compatible overloads
    pointer allocate(size_type n) {
        if (n == 0) return nullptr;
        return static_cast<pointer>(std::malloc(n * sizeof(value_type)));
    }

    void deallocate(pointer p, size_type /*n*/) {
        std::free(p);
    }

    // equality for allocator concept checks
    bool operator==(const MallocAllocator&) const noexcept { return true; }
    bool operator!=(const MallocAllocator& other) const noexcept { return !(*this == other); }

private:
    // ...existing private members if any...
};

} // namespace pool

#endif // H_lib_MallocAllocator
