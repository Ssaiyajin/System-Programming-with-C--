#include "lib/MallocAllocator.hpp"
#include "test/AllocatorConcept.hpp"
#include <cstddef>
#include <cstdint>
#include <unordered_set>
#include <gtest/gtest.h>

// ---------------------------------------------------------------------------
using namespace pool;
using namespace std;

// Properly refer to the allocator defined in the pool namespace
template <typename T>
using MallocAllocator = pool::MallocAllocator<T>;
// ---------------------------------------------------------------------------
namespace {
// ---------------------------------------------------------------------------
template <typename T>
class ScopedMallocAllocator {
private:
    MallocAllocator<T> allocator;
    unordered_set<T*> allocations;

public:
    ~ScopedMallocAllocator() {
        for (T* ptr : allocations)
            allocator.deallocate(ptr);
    }

    T* allocate() {
        T* result = allocator.allocate();
        allocations.insert(result);
        return result;
    }

    void deallocate(T* ptr) {
        allocator.deallocate(ptr);
        allocations.erase(ptr);
    }
};
} // namespace
// ---------------------------------------------------------------------------

TEST(TestMallocAllocator, Concept) {
    struct Foo {
        Foo(const Foo&) = delete;
        Foo(Foo&&) = delete;
        Foo& operator=(const Foo&) = delete;
        Foo& operator=(Foo&&) = delete;
    };

    EXPECT_TRUE(pool::test::IsAllocator<MallocAllocator<int>>);
    EXPECT_TRUE(pool::test::IsAllocator<MallocAllocator<Foo>>);
}
// ---------------------------------------------------------------------------

TEST(TestMallocAllocator, AllocateInt) {
    ScopedMallocAllocator<int> a;
    int* i = a.allocate();
    EXPECT_EQ(reinterpret_cast<uintptr_t>(i) % alignof(int), 0);
    int* j = a.allocate();
    EXPECT_EQ(reinterpret_cast<uintptr_t>(j) % alignof(int), 0);
    EXPECT_NE(i, j);
}
// ---------------------------------------------------------------------------

TEST(TestMallocAllocator, AllocateLargeAlignType) {
    using LargeAlignType = std::max_align_t;
    ScopedMallocAllocator<LargeAlignType> a;
    LargeAlignType* i = a.allocate();
    EXPECT_EQ(reinterpret_cast<uintptr_t>(i) % alignof(LargeAlignType), 0);
    LargeAlignType* j = a.allocate();
    EXPECT_EQ(reinterpret_cast<uintptr_t>(j) % alignof(LargeAlignType), 0);
    EXPECT_NE(i, j);
}
// ---------------------------------------------------------------------------

TEST(TestMallocAllocator, AllocateMany) {
    ScopedMallocAllocator<int> a;
    unordered_set<int*> pointers;
    for (size_t i = 0; i < 1000; ++i) {
        int* p = a.allocate();
        ASSERT_EQ(pointers.count(p), 0);
        pointers.insert(p);
    }
}
// ---------------------------------------------------------------------------
