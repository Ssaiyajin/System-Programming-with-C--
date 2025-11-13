#ifndef H_lib_MallocAllocator
#define H_lib_MallocAllocator
#include <cstdlib>
#include <new>
#include <type_traits>

namespace pool {

template <typename T>
class MallocAllocator {
public:
    using value_type = T;
    using pointer = T*;
    using const_pointer = const T*;
    using void_pointer = void*;
    using const_void_pointer = const void*;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;

    template <class U>
    struct rebind {
        using other = MallocAllocator<U>;
    };

    // -------- Constructors --------
    MallocAllocator() noexcept = default;

    template <class U>
    MallocAllocator(const MallocAllocator<U>&) noexcept {}

    // -------- Allocate n objects --------
    pointer allocate(size_type n) {
        if (n == 0) return nullptr;

        if (n > (size_type(-1) / sizeof(T)))
            throw std::bad_alloc();

        void* p = std::malloc(n * sizeof(T));
        if (!p)
            throw std::bad_alloc();

        return static_cast<pointer>(p);
    }

    // -------- Deallocate n objects --------
    void deallocate(pointer p, size_type) noexcept {
        std::free(p);
    }

    // -------- Comparison --------
    bool operator==(const MallocAllocator&) const noexcept { return true; }
    bool operator!=(const MallocAllocator&) const noexcept { return false; }
};

} // namespace pool