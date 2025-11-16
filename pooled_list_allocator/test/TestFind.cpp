#include "lib/Find.hpp"
#include "lib/List.hpp"
#include "lib/MallocAllocator.hpp"
#include <gtest/gtest.h>
#include <vector>
//---------------------------------------------------------------------------
using namespace std;
//---------------------------------------------------------------------------
namespace allocator_list::test_find {
struct Int {
    int i;

    Int(int i) : i(i) {}
};
bool operator==(const Int& i1, const Int& i2) { return i1.i == i2.i; }
bool operator==(int i1, const Int& i2) { return i1 == i2.i; }
bool operator==(const Int& i1, int i2) { return i1.i == i2; }
bool operator!=(const Int& i1, const Int& i2) { return i1.i != i2.i; }
bool operator!=(int i1, const Int& i2) { return i1 != i2.i; }
bool operator!=(const Int& i1, int i2) { return i1.i != i2; }
}
using namespace allocator_list::test_find;
//---------------------------------------------------------------------------
TEST(TestFind, Vector) {
    vector<int> v{1, 2, 3, 4};
    EXPECT_EQ(pool::find(v, 3), &v[2]);
    EXPECT_EQ(pool::find(v, 123), nullptr);
}
//---------------------------------------------------------------------------
TEST(TestFind, VectorValueType) {
    vector<int> v{1, 2, 3, 4};
    EXPECT_EQ(pool::find(v, Int{3}), &v[2]);
    EXPECT_EQ(pool::find(v, Int{123}), nullptr);
}
//---------------------------------------------------------------------------
TEST(TestFind, List) {
    pool::List<int, pool::MallocAllocator<int>> l;
    l.insert(1);
    l.insert(2);
    l.insert(3);
    l.insert(4);
    EXPECT_TRUE(pool::find(l, 3));
    EXPECT_EQ(pool::find(l, 123), nullptr);
}
//---------------------------------------------------------------------------
TEST(TestFind, ListValueType) {
    pool::List<int, pool::MallocAllocator<int>> l;
    l.insert(1);
    l.insert(2);
    l.insert(3);
    l.insert(4);
    EXPECT_TRUE(pool::find(l, Int{3}));
    EXPECT_EQ(pool::find(l, Int{123}), nullptr);
}
//---------------------------------------------------------------------------
