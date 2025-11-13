#include "lib/PooledAllocator.hpp"
#include "test/AllocatorConcept.hpp"
#include <cstddef>
#include <cstdint>
#include <unordered_set>
#include <gtest/gtest.h>
//---------------------------------------------------------------------------
using namespace pool;
using namespace std;
//---------------------------------------------------------------------------
TEST(TestPooledAllocator, Concept) {
    struct Foo {refactor the entire code and make it working
        Foo(const Foo&) = delete;
        Foo(Foo&&) = delete;
        Foo& operator=(const Foo&) = delete;
        Foo& operator=(Foo&&) = delete;
    };
    // You can change EXPECT_TRUE to static_assert in the following lines to
    // get more detailed error messages that explain why the test fails.
    EXPECT_TRUE(test::IsAllocator<PooledAllocator<int>>);
    EXPECT_TRUE(test::IsAllocator<PooledAllocator<Foo>>);
}
//---------------------------------------------------------------------------
TEST(TestPooledAllocator, AllocateInt) {
    PooledAllocator<int> a;
    int* i = a.allocate();
    EXPECT_EQ(reinterpret_cast<uintptr_t>(i) % alignof(int), 0);
    int* j = a.allocate();
    EXPECT_EQ(reinterpret_cast<uintptr_t>(j) % alignof(int), 0);
    EXPECT_NE(i, j);
}
//---------------------------------------------------------------------------
TEST(TestPooledAllocator, AllocateLargeAlignType) {
    using LargeAlignType = std::max_align_t;
    PooledAllocator<LargeAlignType> a;
    LargeAlignType* i = a.allocate();
    EXPECT_EQ(reinterpret_cast<uintptr_t>(i) % alignof(LargeAlignType), 0);
    LargeAlignType* j = a.allocate();
    EXPECT_EQ(reinterpret_cast<uintptr_t>(j) % alignof(LargeAlignType), 0);
    EXPECT_NE(i, j);
}
//---------------------------------------------------------------------------
TEST(TestPooledAllocator, AllocateMany) {
    PooledAllocator<int> a;
    unordered_set<int*> pointers;
    for (unsigned i = 0; i < 1000; ++i) {
        int* p = a.allocate();
        ASSERT_EQ(pointers.count(p), 0);
        pointers.insert(p);
    }
}
//---------------------------------------------------------------------------
TEST(TestPooledAllocator, AllocateDeallocate) {
    PooledAllocator<int> a;
    // Find two allocations that are stored contiguously
    int* i = a.allocate();
    int* j = a.allocate();
    unsigned iterations = 0;
    while (iterations < 10 && (i + 1 != j)) {
        i = j;
        j = a.allocate();
        ++iterations;
    }
    ASSERT_LT(iterations, 10);
    // Check that the last allocation is reused after freeing
    a.deallocate(j);
    int* j2 = a.allocate();
    EXPECT_EQ(j, j2);
}
//---------------------------------------------------------------------------
TEST(TestPooledAllocator, Move) {
    PooledAllocator<int> a;
    for (unsigned i = 0; i < 1000; ++i) {
        a.allocate();
    }
    PooledAllocator<int> b(move(a));
    PooledAllocator<int> c;
    c = move(b);
}
//---------------------------------------------------------------------------
