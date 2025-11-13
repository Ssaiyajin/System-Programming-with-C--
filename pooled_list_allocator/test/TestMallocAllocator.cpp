#include <gtest/gtest.h>
#include <vector>
#include <cstddef>
#include <memory>

// ✅ Adjust this include if needed
#include "lib/MallocAllocator.hpp"

// Detect correct namespace automatically
namespace pool {
namespace test {

// Try global namespace first — update this line if MallocAllocator is in a namespace
template <typename T>
using MallocAllocator = ::MallocAllocator<T>;

// If your header defines it as `namespace pool { ... }`, then change the above to:
// using MallocAllocator = pool::MallocAllocator<T>;

// ---- Allocator Concept Check ----
template <typename Alloc>
concept AllocatorConcept = requires(Alloc a, typename Alloc::value_type* p) {
    { a.allocate() } -> std::same_as<typename Alloc::value_type*>;
    { a.deallocate(p) };
};

// ---- Helper class to manage allocations safely ----
template <typename T>
class ScopedMallocAllocator {
public:
    ScopedMallocAllocator() = default;
    ~ScopedMallocAllocator() {
        for (T* ptr : allocations) {
            if (ptr)
                allocator.deallocate(ptr);
        }
    }

    T* allocate() {
        T* result = allocator.allocate();
        allocations.push_back(result);
        return result;
    }

    void deallocate(T* ptr) {
        allocator.deallocate(ptr);
    }

private:
    MallocAllocator<T> allocator;
    std::vector<T*> allocations;
};

// Example type
struct Foo {
    int x;
    double y;
};

// ---- TESTS ----
TEST(TestMallocAllocator, Concept_Test) {
    EXPECT_TRUE((AllocatorConcept<MallocAllocator<int>>));
    EXPECT_TRUE((AllocatorConcept<MallocAllocator<Foo>>));
}

TEST(TestMallocAllocator, Allocate_Deallocate_Int) {
    ScopedMallocAllocator<int> scopedAlloc;
    int* ptr = scopedAlloc.allocate();
    ASSERT_NE(ptr, nullptr);
    *ptr = 42;
    EXPECT_EQ(*ptr, 42);
    scopedAlloc.deallocate(ptr);
}

TEST(TestMallocAllocator, Allocate_Deallocate_Struct) {
    ScopedMallocAllocator<Foo> scopedAlloc;
    Foo* ptr = scopedAlloc.allocate();
    ASSERT_NE(ptr, nullptr);
    ptr->x = 10;
    ptr->y = 20.5;
    EXPECT_EQ(ptr->x, 10);
    EXPECT_DOUBLE_EQ(ptr->y, 20.5);
    scopedAlloc.deallocate(ptr);
}

TEST(TestMallocAllocator, MultipleAllocations) {
    ScopedMallocAllocator<int> scopedAlloc;
    int* a = scopedAlloc.allocate();
    int* b = scopedAlloc.allocate();
    ASSERT_NE(a, nullptr);
    ASSERT_NE(b, nullptr);
    EXPECT_NE(a, b);
    *a = 5;
    *b = 15;
    EXPECT_EQ(*a, 5);
    EXPECT_EQ(*b, 15);
    scopedAlloc.deallocate(a);
    scopedAlloc.deallocate(b);
}

} // namespace test
} // namespace pool
