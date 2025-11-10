#include "lib/MultiMap.hpp"
#include <unordered_set>
#include <gtest/gtest.h>
//---------------------------------------------------------------------------
using namespace std;
using namespace hashtable;
//---------------------------------------------------------------------------
namespace {
//---------------------------------------------------------------------------
struct PairHash {
    size_t operator()(const pair<uint32_t, uint32_t>& pair) const {
        return hash<uint32_t>()(pair.first) ^ hash<uint32_t>()(pair.second);
    }
};
//---------------------------------------------------------------------------
} // namespace
//---------------------------------------------------------------------------
TEST(MultiMap, InsertFindUnique) {
    MultiMap map(32);

    for (uint32_t key = 0; key < 32; ++key)
        for (uint32_t value = 0; value < 4; ++value)
            map.insert({key, value});

    for (uint32_t key = 0; key < 32; ++key) {
        unordered_multiset<uint32_t> values;

        auto it = map.findFirst(key);

        while (it != map.end()) {
            values.insert(it->second);
            it = map.findNext(it);
        }

        for (uint32_t value = 0; value < 4; ++value)
            ASSERT_TRUE(values.count(value) == 1);
        ASSERT_EQ(values.size(), 4);
    }
}
//---------------------------------------------------------------------------
TEST(MultiMap, InsertFindNonUnique) {
    MultiMap map(32);

    for (uint32_t key = 0; key < 32; ++key)
        for (uint32_t i = 0; i < 4; ++i)
            map.insert({key, key});

    for (uint32_t key = 0; key < 32; ++key) {
        unordered_multiset<uint32_t> values;

        auto it = map.findFirst(key);

        while (it != map.end()) {
            values.insert(it->second);
            it = map.findNext(it);
        }

        ASSERT_TRUE(values.count(key) == 4);
        ASSERT_EQ(values.size(), 4);
    }
}
//---------------------------------------------------------------------------
TEST(MultiMap, InsertIterateUnique) {
    MultiMap map(32);

    for (uint32_t key = 0; key < 32; ++key)
        for (uint32_t value = 0; value < 4; ++value)
            map.insert({key, value});

    unordered_multiset<pair<uint32_t, uint32_t>, PairHash> reference;
    for (auto& it : map)
        reference.insert({it.first, it.second});

    for (uint32_t key = 0; key < 32; ++key)
        for (uint32_t value = 0; value < 4; ++value)
            ASSERT_EQ(reference.count({key, value}), 1);
    ASSERT_EQ(reference.size(), 32 * 4);
}
//---------------------------------------------------------------------------
TEST(MultiMap, InsertIterateNonUnique) {
    MultiMap map(32);

    for (uint32_t key = 0; key < 32; ++key)
        for (uint32_t i = 0; i < 4; ++i)
            map.insert({key, key});

    unordered_multiset<pair<uint32_t, uint32_t>, PairHash> reference;
    for (auto& it : map)
        reference.insert({it.first, it.second});

    for (uint32_t key = 0; key < 32; ++key)
        ASSERT_EQ(reference.count({key, key}), 4);
    ASSERT_EQ(reference.size(), 32 * 4);
}
//---------------------------------------------------------------------------