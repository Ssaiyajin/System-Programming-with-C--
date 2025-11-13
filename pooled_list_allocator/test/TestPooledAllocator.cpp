#include "lib/PooledAllocator.hpp"
#include "test/AllocatorConcept.hpp"
#include <cstddef>
#include <cstdint>
#include <unordered_set>
#include <gtest/gtest.h>

using namespace std;
using namespace pool;

//----------------------------------------------------------------------------
// Alias the actual allocator class to the expected name
template <typename T>
using PooledAllocator = H_lib_PooledAllocator<T>;

namespace {

// RAII wrapper for PooledAllocator
template <typename T>
class ScopedPooledAllocator {
private:
    PooledAllocator<T> allocator_;
    unordered_set<T*> allocations_;

public:
    ~ScopedPooledAllocator() {
        for (T* ptr : allocations_) {
            allocator_.deallocate(ptr);
        }
    }

    T* allocate() {
        T* ptr = allocator_.allocate();
        allocations_.insert(ptr);
        return ptr;
    }

    void deallocate(T* ptr) {
        if (allocations_.erase(ptr) > 0) {
            allocator_.deallocate(ptr);
        }
    }
};

// Utility function to test pointer alignment
template <typename T>
inline void EXPECT_ALIGNED(T* ptr) {
    EXPECT_EQ(reinterpret_cast<uintptr_t>(ptr) % alignof(T), 0);
}

} // namespace

//===========================================================================
// Concept checks
//===========================================================================
TEST(TestPooledAllocator, Concept) {
    struct Foo {
        Foo(const Foo&) = delete;
        Foo(Foo&&) = delete;
        Foo& operator=(const Foo&) = delete;
        Foo& operator=(Foo&&) = delete;
    };

    EXPECT_TRUE(test::IsAllocator<PooledAllocator<int>>);
    EXPECT_TRUE(test::IsAllocator<PooledAllocator<Foo>>);
}

//===========================================================================
// Allocation tests
//===========================================================================
TEST(TestPooledAllocator, AllocateInt) {
    ScopedPooledAllocator<int> alloc;
    int* i = alloc.allocate();
    int* j = alloc.allocate();

    EXPECT_ALIGNED(i);
    EXPECT_ALIGNED(j);
    EXPECT_NE(i, j);
}

TEST(TestPooledAllocator, AllocateLargeAlignType) {
    using LargeAlignType = std::max_align_t;
    ScopedPooledAllocator<LargeAlignType> alloc;
    LargeAlignType* i = alloc.allocate();
    LargeAlignType* j = alloc.allocate();

    EXPECT_ALIGNED(i);
    EXPECT_ALIGNED(j);
    EXPECT_NE(i, j);
}

TEST(TestPooledAllocator, AllocateMany) {
    ScopedPooledAllocator<int> alloc;
    unordered_set<int*> pointers;

    for (size_t i = 0; i < 1000; ++i) {
        int* ptr = alloc.allocate();
        ASSERT_TRUE(pointers.insert(ptr).second) << "Duplicate allocation detected";
    }
}

TEST(TestPooledAllocator, AllocateDeallocate) {
    ScopedPooledAllocator<int> alloc;
    int* i = alloc.allocate();
    alloc.deallocate(i);

    int* j = alloc.allocate();
    EXPECT_NE(i, j);
}
