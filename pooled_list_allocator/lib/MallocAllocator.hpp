#ifndef H_lib_MallocAllocator
#define H_lib_MallocAllocator

#include <cstdlib>
#include <cstddef>
#include <type_traits>
#include <utility>
#include <new>  

template<typename T>
struct MallocAllocator {
    using value_type = T;

    MallocAllocator() noexcept = default;
    MallocAllocator(const MallocAllocator&) noexcept = default;
    MallocAllocator(MallocAllocator&&) noexcept = default;
    MallocAllocator& operator=(const MallocAllocator&) noexcept = default;
    MallocAllocator& operator=(MallocAllocator&&) noexcept = default;

    template<typename U>
    struct rebind {
        using other = MallocAllocator<U>;
    };

    T* allocate() {
        void* p = std::malloc(sizeof(T));
        if (!p) throw std::bad_alloc();
        return static_cast<T*>(p);
    }

    void deallocate(T* p) noexcept {
        std::free(static_cast<void*>(p));
    }
};

#endif // MALLOC_ALLOCATOR_HPP