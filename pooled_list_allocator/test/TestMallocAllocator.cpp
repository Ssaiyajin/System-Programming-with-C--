#include "lib/MallocAllocator.hpp"
#include <cstddef>
#include <cstdint>
#include <unordered_set>
#include <gtest/gtest.h>

using namespace pool;
using namespace std;

namespace {

template <typename T>
class ScopedMallocAllocator {
private:
    MallocAllocator<T> allocator;
    unordered_set<T*> allocations;

public:
    ~ScopedMallocAllocator() {
        for (auto* ptr : allocations)
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

    size_t live_count() const { return allocations.size(); }
};

} // namespace

TEST(TestMallocAllocator, AllocateInt) {
    ScopedMallocAllocator<int> a;
    int* i = a.allocate();
    ASSERT_NE(i, nullptr);
    EXPECT_EQ(reinterpret_cast<uintptr_t>(i) % alignof(int), 0);
    int* j = a.allocate();
    ASSERT_NE(j, nullptr);
    EXPECT_EQ(reinterpret_cast<uintptr_t>(j) % alignof(int), 0);
    EXPECT_NE(i, j);
}

TEST(TestMallocAllocator, AllocateLargeAlignType) {
    using LargeAlignType = std::max_align_t;
    ScopedMallocAllocator<LargeAlignType> a;
    LargeAlignType* i = a.allocate();
    ASSERT_NE(i, nullptr);
    EXPECT_EQ(reinterpret_cast<uintptr_t>(i) % alignof(LargeAlignType), 0);
    LargeAlignType* j = a.allocate();
    ASSERT_NE(j, nullptr);
    EXPECT_EQ(reinterpret_cast<uintptr_t>(j) % alignof(LargeAlignType), 0);
    EXPECT_NE(i, j);
}

TEST(TestMallocAllocator, AllocateMany) {
    ScopedMallocAllocator<int> a;
    unordered_set<int*> pointers;
    for (size_t k = 0; k < 1000; ++k) {
        int* p = a.allocate();
        ASSERT_NE(p, nullptr);
        ASSERT_EQ(pointers.count(p), 0);
        pointers.insert(p);
    }
    EXPECT_EQ(pointers.size(), 1000u);
}

TEST(TestMallocAllocator, DeallocateAndAllocate) {
    ScopedMallocAllocator<int> a;
    vector<int*> batch;
    for (int i = 0; i < 10; ++i)
        batch.push_back(a.allocate());

    // Deallocate half
    for (int i = 0; i < 5; ++i)
        a.deallocate(batch[i]);

    // allocate some more and ensure alignment and non-null
    unordered_set<int*> live;
    for (int i = 5; i < 10; ++i)
        live.insert(batch[i]);

    for (int i = 0; i < 5; ++i) {
        int* p = a.allocate();
        ASSERT_NE(p, nullptr);
        EXPECT_EQ(reinterpret_cast<uintptr_t>(p) % alignof(int), 0);
        // ensure not colliding with currently live pointers
        ASSERT_EQ(live.count(p), 0);
        live.insert(p);
    }

    EXPECT_EQ(a.live_count(), live.size());
}
