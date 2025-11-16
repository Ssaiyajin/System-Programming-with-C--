#ifndef H_lib_MallocAllocator.hpp"
#define H_lib_MallocAllocatorpt.hpp"
#pragma oncetddef>
#include <cstddef>
#include <cstdlib>d_set>
#include <new>t/gtest.h>
#include <type_traits>-------------------------------------------------------
using namespace pool;
namespace pool {std;
//---------------------------------------------------------------------------
template <typename T>
class MallocAllocator {------------------------------------------------------
public:e <typename T>
    // Standard allocator typedefs required by std::allocator_traits / concepts
    using value_type = T;
    using pointer = T*;allocator;
    using const_pointer = const T*;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;
    ~ScopedMallocAllocator() {
    // rebind (old-style) and conversion ctorions)
    template <typename U>llocate(ptr);
    struct rebind { using other = MallocAllocator<U>; };

    MallocAllocator() noexcept = default;
    template <typename U>        T* result = allocator.allocate();
    MallocAllocator(const MallocAllocator<U>&) noexcept {}t(result);

    // allocator_traits-compatible interface    }
    pointer allocate(size_type n) {
        if (n == 0) return nullptr;
        void* p = std::malloc(n * sizeof(value_type));
        if (!p) throw std::bad_alloc();
        return static_cast<pointer>(p);
    }
------------------------------------------------------------------------
    void deallocate(pointer p, size_type /*n*/) noexcept {} // namespace
        std::free(p);-------------------
    }tor, Concept) {
truct Foo {
    // Convenience single-object allocation used by tests        Foo(const Foo&) = delete;
    pointer allocate() {
        return allocate(1);onst Foo&) = delete;
    }&) = delete;
;
    void deallocate(pointer p) noexcept {    // You can change EXPECT_TRUE to static_assert in the following lines to
        deallocate(p, 1);hat explain why the test fails.
    }llocator<MallocAllocator<int>>);
XPECT_TRUE(test::IsAllocator<MallocAllocator<Foo>>);
    // equality for allocator concepts}
    bool operator==(const MallocAllocator&) const noexcept { return true; }--------------------------
    bool operator!=(const MallocAllocator& other) const noexcept { return !(*this == other); }
};
  int* i = a.allocate();
} // namespace pool    EXPECT_EQ(reinterpret_cast<uintptr_t>(i) % alignof(int), 0);
cate();
#endif // H_lib_MallocAllocator    EXPECT_EQ(reinterpret_cast<uintptr_t>(j) % alignof(int), 0);
//---------------------------------------------------------------------------
TEST(TestMallocAllocator, AllocateLargeAlignType) {
    using LargeAlignType = std::max_align_t;
    ScopedMallocAllocator<LargeAlignType> a;
    LargeAlignType* i = a.allocate();
    EXPECT_EQ(reinterpret_cast<uintptr_t>(i) % alignof(LargeAlignType), 0);
    LargeAlignType* j = a.allocate();
    EXPECT_EQ(reinterpret_cast<uintptr_t>(j) % alignof(LargeAlignType), 0);
    EXPECT_NE(i, j);
}
//---------------------------------------------------------------------------
TEST(TestMallocAllocator, AllocateMany) {
    ScopedMallocAllocator<int> a;
    unordered_set<int*> pointers;
    for (size_t i = 0; i < 1000; ++i) {
        int* p = a.allocate();
        ASSERT_EQ(pointers.count(p), 0);
        pointers.insert(p);
    }
}
//---------------------------------------------------------------------------
