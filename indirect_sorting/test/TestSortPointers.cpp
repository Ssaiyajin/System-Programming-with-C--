#include "lib/SortPointers.hpp"
#include <numeric>
#include <random>
#include <algorithm>   // <--- added
#include <gtest/gtest.h>
//---------------------------------------------------------------------------
using namespace std;
using namespace sort_pointers;
//---------------------------------------------------------------------------
namespace {
//---------------------------------------------------------------------------
pair<vector<unsigned>, vector<const unsigned*>> generate(unsigned n)
// Generate shuffled data
{
    vector<unsigned> data(n);
    iota(data.begin(), data.end(), 0);
    std::shuffle(data.begin(), data.end(), mt19937(42)); // qualify shuffle

    vector<const unsigned*> ptrs;
    ptrs.reserve(data.size());

    for (const auto& e : data)
        ptrs.push_back(&e);

    return std::make_pair(std::move(data), std::move(ptrs)); // qualify move/make_pair
}
//---------------------------------------------------------------------------
} // namespace
//---------------------------------------------------------------------------
TEST(SortTest, quicksortSingle) {
    auto [data, ptrs] = generate(1);

    quicksort(&ptrs[0], &ptrs[ptrs.size()]);

    for (unsigned i = 0; i < data.size(); ++i)
        ASSERT_EQ(*ptrs[i], i);
}
//---------------------------------------------------------------------------
TEST(SortTest, quicksortSmall) {
    auto [data, ptrs] = generate(25);

    quicksort(&ptrs[0], &ptrs[ptrs.size()]);

    for (unsigned i = 0; i < data.size(); ++i)
        ASSERT_EQ(*ptrs[i], i);
}
//---------------------------------------------------------------------------
TEST(SortTest, quicksortLarge) {
    auto [data, ptrs] = generate(1000);

    quicksort(&ptrs[0], &ptrs[ptrs.size()]);

    for (unsigned i = 0; i < data.size(); ++i)
        ASSERT_EQ(*ptrs[i], i);
}
//---------------------------------------------------------------------------
TEST(SortTest, mergesortSingle) {
    auto [data, ptrs] = generate(1);

    mergesort(&ptrs[0], &ptrs[ptrs.size()]);

    for (unsigned i = 0; i < data.size(); ++i)
        ASSERT_EQ(*ptrs[i], i);
}
//---------------------------------------------------------------------------
TEST(SortTest, mergesortSmall) {
    auto [data, ptrs] = generate(25);

    mergesort(&ptrs[0], &ptrs[ptrs.size()]);

    for (unsigned i = 0; i < data.size(); ++i)
        ASSERT_EQ(*ptrs[i], i);
}
//---------------------------------------------------------------------------
TEST(SortTest, mergesortLarge) {
    auto [data, ptrs] = generate(1000);

    mergesort(&ptrs[0], &ptrs[ptrs.size()]);

    for (unsigned i = 0; i < data.size(); ++i)
        ASSERT_EQ(*ptrs[i], i);
}
//---------------------------------------------------------------------------
