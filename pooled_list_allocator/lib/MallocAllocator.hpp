#ifndef H_lib_MallocAllocator
#define H_lib_MallocAllocator
#pragma once
#include <cstddef>
#include <cstdlib>
#include <new>
#include <type_traits>
#include <limits>
#include <utility>

namespace pool {

template <typename T>
class MallocAllocator {
public:
    using value_type = T;
    using pointer = T*;
    using const_pointer = const T*;
    using reference = T&;
    using const_reference = const T&;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;

    // old-style rebind
    template <typename U>
    struct rebind { using other = MallocAllocator<U>; };

    // traits used by some concepts
    using propagate_on_container_copy_assignment = std::false_type;
    using propagate_on_container_move_assignment = std::true_type;
    using propagate_on_container_swap = std::false_type;
    using is_always_equal = std::true_type;

    MallocAllocator() noexcept = default;

    template <typename U>
    MallocAllocator(const MallocAllocator<U>&) noexcept {}

    // allocate/deallocate expected by allocator_traits
    pointer allocate(size_type n) {
        if (n == 0) return nullptr;
        void* p = std::malloc(n * sizeof(value_type));
        if (!p) throw std::bad_alloc();
        return static_cast<pointer>(p);
    }

    void deallocate(pointer p, size_type /*n*/) noexcept {
        std::free(p);
    }

    // convenience single-object API used by tests
    pointer allocate() {
        return allocate(1);
    }

    void deallocate(pointer p) noexcept {
        deallocate(p, 1);
    }

    // Construct/destroy (some allocator concepts check for these)
    template <typename U, typename... Args>
    void construct(U* p, Args&&... args) {
        ::new (static_cast<void*>(p)) U(std::forward<Args>(args)...);
    }

    template <typename U>
    void destroy(U* p) noexcept {
        p->~U();
    }

    // Helpers
    size_type max_size() const noexcept {
        return std::numeric_limits<size_type>::max() / sizeof(value_type);
    }

    // equality for allocator concepts
    bool operator==(const MallocAllocator&) const noexcept { return true; }
    bool operator!=(const MallocAllocator& other) const noexcept { return !(*this == other); }
};

} // namespace pool

#endif // H_lib_MallocAllocator
