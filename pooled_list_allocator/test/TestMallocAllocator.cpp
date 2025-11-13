#include "PooledAllocator.hpp"       // your allocator header
#include "test/AllocatorConcept.hpp" // contains test::IsAllocator
#include <gtest/gtest.h>
#include <unordered_set>
#include <vector>
#include <cstdint>
#include <cstddef>
#include <algorithm>

// Use your allocator namespace
using namespace H_lib; // adjust this to match the namespace of H_lib_PooledAllocator

// Template alias for convenience
template <typename T>
using PooledAllocator = H_lib_PooledAllocator<T>;

// Scoped allocator for testing
template <typename T>
class ScopedPooledAllocator {
private:
    PooledAllocator<T> allocator_;
    std::unordered_set<T*> allocations_; // faster erase than vector

public:
    ~ScopedPooledAllocator() {
        for (T* ptr : allocations_)
            allocator_.deallocate(ptr);
    }

    T* allocate() {
        T* ptr = allocator_.allocate();
        allocations_.insert(ptr);
        return ptr;
    }

    void deallocate(T* ptr) {
        allocator_.deallocate(ptr);
        allocations_.erase(ptr);
    }
};

// Dummy struct for testing
struct Foo {
    int a;
    double b;
};

// -------------------- Google Tests --------------------

TEST(TestPooledAllocator, Concept) {
    // Check if the allocator satisfies the Allocator concept
    EXPECT_TRUE((test::IsAllocator<PooledAllocator<int>>));
    EXPECT_TRUE((test::IsAllocator<PooledAllocator<Foo>>));
}

TEST(TestPooledAllocator, AllocateInt) {
    ScopedPooledAllocator<int> a;
    int* i = a.allocate();
    EXPECT_EQ(reinterpret_cast<uintptr_t>(i) % alignof(int), 0);
    int* j = a.allocate();
    EXPECT_EQ(reinterpret_cast<uintptr_t>(j) % alignof(int), 0);
    EXPECT_NE(i, j);

    a.deallocate(i);
    a.deallocate(j);
}

TEST(TestPooledAllocator, AllocateLargeAlignType) {
    using LargeAlignType = std::max_align_t;
    ScopedPooledAllocator<LargeAlignType> a;
    LargeAlignType* i = a.allocate();
    EXPECT_EQ(reinterpret_cast<uintptr_t>(i) % alignof(LargeAlignType), 0);
    LargeAlignType* j = a.allocate();
    EXPECT_EQ(reinterpret_cast<uintptr_t>(j) % alignof(LargeAlignType), 0);
    EXPECT_NE(i, j);

    a.deallocate(i);
    a.deallocate(j);
}

TEST(TestPooledAllocator, AllocateMany) {
    ScopedPooledAllocator<int> a;
    std::unordered_set<int*> pointers;

    for (size_t i = 0; i < 1000; ++i) {
        int* p = a.allocate();
        ASSERT_EQ(pointers.count(p), 0); // ensure uniqueness
        pointers.insert(p);
    }
}

TEST(TestPooledAllocator, AllocateAndDeallocate) {
    ScopedPooledAllocator<Foo> a;
    std::vector<Foo*> allocated;

    // Allocate 100 objects
    for (int i = 0; i < 100; ++i) {
        allocated.push_back(a.allocate());
    }

    // Deallocate all
    for (Foo* f : allocated) {
        a.deallocate(f);
    }

    allocated.clear();

    // Allocate again to test reuse
    for (int i = 0; i < 100; ++i) {
        allocated.push_back(a.allocate());
    }
}
