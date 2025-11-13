#include "lib/MallocAllocator.hpp"
#include "test/AllocatorConcept.hpp"
#include <cstddef>
#include <cstdint>
#include <unordered_set>
#include <gtest/gtest.h>

using namespace pool;
using namespace std;

namespace {

// RAII wrapper for MallocAllocator
template <typename T>
class ScopedMallocAllocator {
private:
    MallocAllocator<T> allocator_;
    unordered_set<T*> allocations_;

public:
    ~ScopedMallocAllocator() {
        for (T* ptr : allocations_) {
            allocator_.deallocate(ptr);
        }
    }

    // Allocate memory for one element
    T* allocate() {
        T* ptr = allocator_.allocate();
        allocations_.insert(ptr);
        return ptr;
    }

    // Deallocate a specific pointer
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
TEST(TestMallocAllocator, Concept) {
    struct Foo {
        Foo(const Foo&) = delete;
        Foo(Foo&&) = delete;
        Foo& operator=(const Foo&) = delete;
        Foo& operator=(Foo&&) = delete;
    };

    EXPECT_TRUE(test::IsAllocator<MallocAllocator<int>>);
    EXPECT_TRUE(test::IsAllocator<MallocAllocator<Foo>>);
}

//===========================================================================
// Allocation tests
//===========================================================================
TEST(TestMallocAllocator, AllocateInt) {
    ScopedMallocAllocator<int> alloc;
    int* i = alloc.allocate();
    int* j = alloc.allocate();

    EXPECT_ALIGNED(i);
    EXPECT_ALIGNED(j);
    EXPECT_NE(i, j);
}

TEST(TestMallocAllocator, AllocateLargeAlignType) {
    using LargeAlignType = std::max_align_t;
    ScopedMallocAllocator<LargeAlignType> alloc;
    LargeAlignType* i = alloc.allocate();
    LargeAlignType* j = alloc.allocate();

    EXPECT_ALIGNED(i);
    EXPECT_ALIGNED(j);
    EXPECT_NE(i, j);
}

TEST(TestMallocAllocator, AllocateMany) {
    ScopedMallocAllocator<int> alloc;
    unordered_set<int*> pointers;

    for (size_t i = 0; i < 1000; ++i) {
        int* ptr = alloc.allocate();
        ASSERT_TRUE(pointers.insert(ptr).second) << "Duplicate allocation detected";
    }
}
