#pragma once

#include <cstdlib>
#include <new>
#include <cstddef>

namespace pool {

template <typename T>
struct MallocAllocator {
    using value_type = T;

    MallocAllocator() noexcept = default;

    // Allocate and value-initialize a single T. Returns nullptr on OOM.
    T* allocate() noexcept {
        void* mem = std::malloc(sizeof(T));
        if (!mem) return nullptr;
        // value-initialize the object in-place
        return new (mem) T();
    }

    // Destroy and free memory.
    void deallocate(T* p) noexcept {
        if (!p) return;
        p->~T();
        std::free(p);
    }
};

} // namespace pool
