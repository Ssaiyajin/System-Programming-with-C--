#include "lib/List.hpp"
#include "lib/MallocAllocator.hpp"
#include "test/ListConcept.hpp"
#include <optional>
#include <utility>
#include <gtest/gtest.h>
//---------------------------------------------------------------------------
using namespace pool;
using namespace std;
//---------------------------------------------------------------------------
namespace {
//---------------------------------------------------------------------------
struct Foo {
    int i;

    Foo(int i) : i(i) {}
    Foo(const Foo&) = delete;
    Foo& operator=(const Foo&) = delete;
    Foo(Foo&&) = default;
    Foo& operator=(Foo&&) = default;
};
//---------------------------------------------------------------------------
using IntList = List<int, MallocAllocator<int>>;
using FooList = List<Foo, MallocAllocator<Foo>>;
//---------------------------------------------------------------------------
} // namespace
//---------------------------------------------------------------------------
TEST(TestList, Concept) {
    // You can change EXPECT_TRUE to static_assert in the following lines to
    // get more detailed error messages that explain why the test fails.
    EXPECT_TRUE(test::IsList<IntList>);
    EXPECT_TRUE(test::CopyableList<IntList>);
    EXPECT_TRUE(test::ComparableList<IntList>);
    EXPECT_TRUE(test::IsList<FooList>);
    EXPECT_TRUE(!test::CopyableList<FooList>);
    EXPECT_TRUE(!test::ComparableList<FooList>);
}
//---------------------------------------------------------------------------
TEST(TestList, InsertErase) {
    IntList l;
    EXPECT_EQ(l.size(), 0);
    for (unsigned i = 1; i <= 100; ++i) {
        l.insert(i);
        EXPECT_EQ(l.size(), i);
    }
    for (unsigned i = 1; i <= 100; ++i) {
        l.erase(i);
        EXPECT_EQ(l.size(), 100 - i);
    }
}
//---------------------------------------------------------------------------
TEST(TestList, InsertEraseReverse) {
    IntList l;
    EXPECT_EQ(l.size(), 0);
    for (unsigned i = 1; i <= 100; ++i) {
        l.insert(i);
        EXPECT_EQ(l.size(), i);
    }
    for (unsigned i = 1; i <= 100; ++i) {
        l.erase(100 - i + 1);
        EXPECT_EQ(l.size(), 100 - i);
    }
}
//---------------------------------------------------------------------------
TEST(TestList, Duplicates) {
    IntList l;
    EXPECT_EQ(l.size(), 0);
    for (unsigned i = 1; i <= 100; ++i) {
        l.insert(123);
        EXPECT_EQ(l.size(), i);
    }
    for (unsigned i = 1; i <= 100; ++i) {
        l.erase(123);
        EXPECT_EQ(l.size(), 100 - i);
    }
}
//---------------------------------------------------------------------------
TEST(TestList, EraseFromEmpty) {
    IntList l;
    EXPECT_EQ(l.size(), 0);
    l.erase(123);
    EXPECT_EQ(l.size(), 0);
}
//---------------------------------------------------------------------------
TEST(TestList, EraseNotExisting) {
    IntList l;
    EXPECT_EQ(l.size(), 0);
    l.insert(123);
    EXPECT_EQ(l.size(), 1);
    l.erase(456);
    EXPECT_EQ(l.size(), 1);
}
//---------------------------------------------------------------------------
TEST(TestList, Copy) {
    IntList l;
    EXPECT_EQ(l.size(), 0);
    l.insert(123);
    EXPECT_EQ(l.size(), 1);

    IntList l2(l);
    EXPECT_EQ(l.size(), 1);
    EXPECT_EQ(l2.size(), 1);

    IntList l3;
    l3 = l;
    EXPECT_EQ(l.size(), 1);
    EXPECT_EQ(l3.size(), 1);
}
//---------------------------------------------------------------------------
TEST(TestList, Move) {
    IntList l;
    EXPECT_EQ(l.size(), 0);
    l.insert(123);
    EXPECT_EQ(l.size(), 1);

    IntList l2(std::move(l));
    // check the moved-to object rather than using the moved-from 'l'
    EXPECT_EQ(l2.size(), 0);

    IntList l3;
    l3 = std::move(l2);
    // check the moved-to object rather than using the moved-from 'l2'
    EXPECT_EQ(l3.size(), 0);
}
//---------------------------------------------------------------------------
namespace {
//---------------------------------------------------------------------------
static unsigned allocateCalls = 0;
static unsigned deallocateCalls = 0;
static unsigned defaultConstructorCalls = 0;
static unsigned copyConstructorCalls = 0;
static unsigned destructorCalls = 0;
//---------------------------------------------------------------------------
void resetCounters() {
    allocateCalls = 0;
    deallocateCalls = 0;
    defaultConstructorCalls = 0;
    copyConstructorCalls = 0;
    destructorCalls = 0;
}
//---------------------------------------------------------------------------
template <typename T>
class TestAllocator {
public:
    using value_type = T;
    using pointer = T*;
    using const_pointer = const T*;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;

    template <typename U>
    struct rebind { using other = TestAllocator<U>; };

    MallocAllocator<T> allocator;

    TestAllocator() noexcept = default;

    template <typename U>
    TestAllocator(const TestAllocator<U>&) noexcept {}

    // Standard allocator interface
    pointer allocate(size_type n) {
        ++allocateCalls;
        return allocator.allocate(n);
    }

    void deallocate(pointer p, size_type n) {
        ++deallocateCalls;
        allocator.deallocate(p, n);
    }

    bool operator==(const TestAllocator&) const noexcept { return true; }
    bool operator!=(const TestAllocator&) const noexcept { return false; }
};
//---------------------------------------------------------------------------
class TrackedStructors {
    public:
    TrackedStructors() { ++defaultConstructorCalls; }
    TrackedStructors(const TrackedStructors&) { ++copyConstructorCalls; }
    ~TrackedStructors() { ++destructorCalls; }
};
//---------------------------------------------------------------------------
bool operator==(const TrackedStructors&, const TrackedStructors&) {
    return true;
}
bool operator!=(const TrackedStructors&, const TrackedStructors&) {
    return false;
}
//---------------------------------------------------------------------------
} // namespace
//---------------------------------------------------------------------------
void testListAllocation_dummyEqualityOperators() {
    // This code exists only to remove warnings about the unused comparison
    // functions
    // Compare two distinct objects to avoid the "both sides ... are equivalent"
    // warning that arises when comparing the same variable to itself.
    TrackedStructors t1;
    TrackedStructors t2;
    static_cast<void>(t1 == t2);
    static_cast<void>(t1 != t2);
}
//---------------------------------------------------------------------------
TEST(TestList, Allocation) {
    resetCounters();
    optional<List<TrackedStructors, TestAllocator<TrackedStructors>>> l;
    l.emplace();

    EXPECT_EQ(allocateCalls, 0);
    EXPECT_EQ(deallocateCalls, 0);
    EXPECT_EQ(defaultConstructorCalls, 0);
    EXPECT_EQ(copyConstructorCalls, 0);
    EXPECT_EQ(destructorCalls, 0);

    TrackedStructors t;
    l->insert(t);

    EXPECT_EQ(allocateCalls, 1);
    EXPECT_EQ(deallocateCalls, 0);
    EXPECT_EQ(defaultConstructorCalls, 1);
    EXPECT_EQ(copyConstructorCalls, 1);
    EXPECT_EQ(destructorCalls, 0);

    l->erase(t);

    EXPECT_EQ(allocateCalls, 1);
    EXPECT_EQ(deallocateCalls, 1);
    EXPECT_EQ(defaultConstructorCalls, 1);
    EXPECT_EQ(copyConstructorCalls, 1);
    EXPECT_EQ(destructorCalls, 1);

    for (unsigned i = 0; i < 100; ++i) {
        l->insert(t);
    }

    EXPECT_EQ(allocateCalls, 101);
    EXPECT_EQ(deallocateCalls, 1);
    EXPECT_EQ(defaultConstructorCalls, 1);
    EXPECT_EQ(copyConstructorCalls, 101);
    EXPECT_EQ(destructorCalls, 1);

    l.reset();

    EXPECT_EQ(allocateCalls, 101);
    EXPECT_EQ(deallocateCalls, 101);
    EXPECT_EQ(defaultConstructorCalls, 1);
    EXPECT_EQ(copyConstructorCalls, 101);
    EXPECT_EQ(destructorCalls, 101);
}
//---------------------------------------------------------------------------
TEST(TestList, AllocationCopy) {
    resetCounters();
    optional<List<TrackedStructors, TestAllocator<TrackedStructors>>> l1, l2, l3;
    l1.emplace();
    l2.emplace();

    EXPECT_EQ(allocateCalls, 0);
    EXPECT_EQ(deallocateCalls, 0);
    EXPECT_EQ(defaultConstructorCalls, 0);
    EXPECT_EQ(copyConstructorCalls, 0);
    EXPECT_EQ(destructorCalls, 0);

    TrackedStructors t;
    for (unsigned i = 0; i < 10; ++i) {
        l1->insert(t);
    }

    EXPECT_EQ(allocateCalls, 10);
    EXPECT_EQ(deallocateCalls, 0);
    EXPECT_EQ(defaultConstructorCalls, 1);
    EXPECT_EQ(copyConstructorCalls, 10);
    EXPECT_EQ(destructorCalls, 0);

    for (unsigned i = 0; i < 10; ++i) {
        l2->insert(t);
    }

    EXPECT_EQ(allocateCalls, 20);
    EXPECT_EQ(deallocateCalls, 0);
    EXPECT_EQ(defaultConstructorCalls, 1);
    EXPECT_EQ(copyConstructorCalls, 20);
    EXPECT_EQ(destructorCalls, 0);

    // Use copy constructor
    l3.emplace(*l2);

    EXPECT_EQ(allocateCalls, 30);
    EXPECT_EQ(deallocateCalls, 0);
    EXPECT_EQ(defaultConstructorCalls, 1);
    EXPECT_EQ(copyConstructorCalls, 30);
    EXPECT_EQ(destructorCalls, 0);

    // Use copy assignment
    *l1 = *l2;

    EXPECT_EQ(allocateCalls, 40);
    EXPECT_EQ(deallocateCalls, 10);
    EXPECT_EQ(defaultConstructorCalls, 1);
    EXPECT_EQ(copyConstructorCalls, 40);
    EXPECT_EQ(destructorCalls, 10);

    l1.reset();
    l2.reset();
    l3.reset();

    EXPECT_EQ(allocateCalls, 40);
    EXPECT_EQ(deallocateCalls, 40);
    EXPECT_EQ(defaultConstructorCalls, 1);
    EXPECT_EQ(copyConstructorCalls, 40);
    EXPECT_EQ(destructorCalls, 40);
}
//---------------------------------------------------------------------------
TEST(TestList, AllocationMove) {
    resetCounters();
    optional<List<TrackedStructors, TestAllocator<TrackedStructors>>> l1, l2, l3;
    l1.emplace();
    l2.emplace();

    EXPECT_EQ(allocateCalls, 0);
    EXPECT_EQ(deallocateCalls, 0);
    EXPECT_EQ(defaultConstructorCalls, 0);
    EXPECT_EQ(copyConstructorCalls, 0);
    EXPECT_EQ(destructorCalls, 0);

    TrackedStructors t;
    for (unsigned i = 0; i < 10; ++i) {
        l1->insert(t);
    }

    EXPECT_EQ(allocateCalls, 10);
    EXPECT_EQ(deallocateCalls, 0);
    EXPECT_EQ(defaultConstructorCalls, 1);
    EXPECT_EQ(copyConstructorCalls, 10);
    EXPECT_EQ(destructorCalls, 0);

    for (unsigned i = 0; i < 10; ++i) {
        l2->insert(t);
    }

    EXPECT_EQ(allocateCalls, 20);
    EXPECT_EQ(deallocateCalls, 0);
    EXPECT_EQ(defaultConstructorCalls, 1);
    EXPECT_EQ(copyConstructorCalls, 20);
    EXPECT_EQ(destructorCalls, 0);

    // Use move constructor
    l3.emplace(std::move(*l2));

    EXPECT_EQ(allocateCalls, 20);
    EXPECT_EQ(deallocateCalls, 0);
    EXPECT_EQ(defaultConstructorCalls, 1);
    EXPECT_EQ(copyConstructorCalls, 20);
    EXPECT_EQ(destructorCalls, 0);

    // Use move assignment
    *l2 = std::move(*l1);

    EXPECT_EQ(allocateCalls, 20);
    EXPECT_EQ(deallocateCalls, 0);
    EXPECT_EQ(defaultConstructorCalls, 1);
    EXPECT_EQ(copyConstructorCalls, 20);
    EXPECT_EQ(destructorCalls, 0);

    l1.reset();
    l2.reset();
    l3.reset();

    EXPECT_EQ(allocateCalls, 20);
    EXPECT_EQ(deallocateCalls, 20);
    EXPECT_EQ(defaultConstructorCalls, 1);
    EXPECT_EQ(copyConstructorCalls, 20);
    EXPECT_EQ(destructorCalls, 20);
}
//---------------------------------------------------------------------------
TEST(TestListExtra, MoveTransferContents) {
    using IntList = List<int, MallocAllocator<int>>;
    IntList l;
    EXPECT_EQ(l.size(), 0u);
    l.insert(1);
    EXPECT_EQ(l.size(), 1u);
    l.insert(2);
    EXPECT_EQ(l.size(), 2u);
    l.insert(3);
    EXPECT_EQ(l.size(), 3u);

    // move-construct
    IntList moved(std::move(l));

    // moved should have the elements, moved-from should be empty
    EXPECT_EQ(moved.size(), 3u);
    EXPECT_EQ(l.size(), 0u);
}

TEST(TestListExtra, MoveAssignmentTransfer) {
    using IntList = List<int, MallocAllocator<int>>;
    IntList a;
    EXPECT_EQ(a.size(), 0u);
    a.insert(42);
    EXPECT_EQ(a.size(), 1u);

    IntList b;
    b = std::move(a);

    EXPECT_EQ(b.size(), 1u);
    EXPECT_EQ(a.size(), 0u);
}

TEST(TestListExtra, MoveDoesNotDuplicate) {
    using IntList = List<int, MallocAllocator<int>>;
    IntList src;
    src.insert(7);
    EXPECT_EQ(src.size(), 1u);

    IntList dst(std::move(src));
    EXPECT_EQ(dst.size(), 1u);
    EXPECT_EQ(src.size(), 0u);

    // Mutate dst and ensure src remains empty
    dst.insert(8);
    EXPECT_EQ(dst.size(), 2u);
    EXPECT_EQ(src.size(), 0u);
}

TEST(MoveSemanticsTest, MoveConstructorTransfersContents) {
    IntList src;
    EXPECT_EQ(src.size(), 0u);
    src.insert(123);
    EXPECT_EQ(src.size(), 1u);
    src.insert(456);
    EXPECT_EQ(src.size(), 2u);

    IntList dst(std::move(src));

    // dst should acquire the elements, src should become empty
    EXPECT_EQ(dst.size(), 2u);
    EXPECT_EQ(src.size(), 0u);
}

TEST(MoveSemanticsTest, MoveAssignmentTransfersContents) {
    IntList a;
    EXPECT_EQ(a.size(), 0u);
    a.insert(42);
    EXPECT_EQ(a.size(), 1u);

    IntList b;
    b = std::move(a);

    EXPECT_EQ(b.size(), 1u);
    EXPECT_EQ(a.size(), 0u);
}

TEST(MoveSemanticsTest, MoveDoesNotDuplicateAfterMutation) {
    IntList src;
    src.insert(7);
    EXPECT_EQ(src.size(), 1u);

    IntList dst(std::move(src));
    EXPECT_EQ(dst.size(), 1u);
    EXPECT_EQ(src.size(), 0u);

    dst.insert(8);
    EXPECT_EQ(dst.size(), 2u);
    EXPECT_EQ(src.size(), 0u);
}

TEST(MoveSemanticsTest, SelfMoveAssignmentIsSafe) {
    using IntList = List<int, MallocAllocator<int>>;

    IntList x;
    x.insert(1);
    x.insert(2);
    const auto before = x.size();

    // Avoid doing `x = std::move(x);` which triggers -Werror=self-move.
    // Check that self copy-assignment is safe
    x = x;
    EXPECT_EQ(x.size(), before);

    // Check move-assignment from a distinct moved-from object
    IntList tmp = x;
    x = std::move(tmp);
    EXPECT_EQ(x.size(), before);
}
//---------------------------------------------------------------------------