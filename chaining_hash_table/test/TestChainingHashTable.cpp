#include "lib/ChainingHashTable.hpp"
#include <cstring>
#include <iterator>
#include <memory>
#include <type_traits>
#include <utility>
#include <gtest/gtest.h>
//---------------------------------------------------------------------------
using namespace hashtable;
using namespace std;
//---------------------------------------------------------------------------
static GenericValue GV(int i) {
    GenericValue value;
    memcpy(value.getData(), &i, sizeof(i));
    return value;
}
//---------------------------------------------------------------------------
static int asInt(const GenericValue& gv) {
    int value;
    memcpy(&value, gv.getData(), sizeof(value));
    return value;
}
//---------------------------------------------------------------------------
TEST(TestChainingHashTable, EntryKeyIsConst) {
    ASSERT_TRUE(is_const_v<decltype(ChainingHashTable::Entry::key)>);
}
//---------------------------------------------------------------------------
TEST(TestChainingHashTable, ConstructEmpty) {
    ChainingHashTable ht;
    ASSERT_EQ(ht.size(), 0);
}
//---------------------------------------------------------------------------
TEST(TestChainingHashTable, InsertContains) {
    ChainingHashTable ht;
    EXPECT_EQ(ht.size(), 0);
    ht.insert(123, GV(456));
    EXPECT_EQ(ht.size(), 1);
    EXPECT_TRUE(ht.contains(123));
}
//---------------------------------------------------------------------------
TEST(TestChainingHashTable, InsertContainsConst) {
    ChainingHashTable ht;
    const ChainingHashTable& constHt = ht;
    EXPECT_EQ(constHt.size(), 0);
    ht.insert(123, GV(456));
    EXPECT_EQ(constHt.size(), 1);
    EXPECT_TRUE(constHt.contains(123));
}
//---------------------------------------------------------------------------
TEST(TestChainingHashTable, InsertLookup) {
    ChainingHashTable ht;
    EXPECT_EQ(ht.size(), 0);
    ht.insert(123, GV(456));
    EXPECT_EQ(ht.size(), 1);
    EXPECT_EQ(asInt(ht[123]), 456);
}
//---------------------------------------------------------------------------
TEST(TestChainingHashTable, SubscriptInsert) {
    ChainingHashTable ht;
    EXPECT_EQ(ht.size(), 0);

    ht[123] = GV(456);
    EXPECT_EQ(ht.size(), 1);
    EXPECT_TRUE(ht.contains(123));

    ht[234] = GV(567);
    EXPECT_EQ(ht.size(), 2);
    EXPECT_TRUE(ht.contains(234));

    ht[123] = GV(789);
    EXPECT_EQ(ht.size(), 2);
    EXPECT_EQ(asInt(ht[123]), 789);
}
//---------------------------------------------------------------------------
TEST(TestChainingHashTable, InsertMany) {
    ChainingHashTable ht;
    for (int i = 0; i < 1000; ++i) {
        SCOPED_TRACE(i);
        int value = (i + 43) * 1327;
        ht.insert(value, GV(i));
        EXPECT_EQ(ht.size(), i + 1);
        EXPECT_TRUE(ht.contains(value));
    }
    for (int i = 0; i < 1000; ++i) {
        SCOPED_TRACE(i);
        int value = (i + 43) * 1327;
        EXPECT_TRUE(ht.contains(value));
        EXPECT_EQ(asInt(ht[value]), i);
    }
}
//---------------------------------------------------------------------------
TEST(TestChainingHashTable, InsertErase) {
    ChainingHashTable ht;
    for (int i = 0; i < 100; ++i) {
        SCOPED_TRACE(i);
        ht.insert(i, GV(i));
        EXPECT_EQ(ht.size(), i + 1);
    }
    for (int i = 0; i < 100; ++i) {
        SCOPED_TRACE(i);
        EXPECT_TRUE(ht.contains(i));
        ht.erase(i);
        EXPECT_FALSE(ht.contains(i));
        EXPECT_EQ(ht.size(), 100 - i - 1);
    }
}
//---------------------------------------------------------------------------
TEST(TestChainingHashTable, Move) {
    ChainingHashTable ht1;
    ht1.insert(123, GV(456));
    ht1.insert(234, GV(567));

    EXPECT_EQ(ht1.size(), 2);
    EXPECT_TRUE(ht1.contains(123));
    EXPECT_TRUE(ht1.contains(234));

    ChainingHashTable ht2(move(ht1));
    EXPECT_EQ(ht2.size(), 2);
    EXPECT_TRUE(ht2.contains(123));
    EXPECT_TRUE(ht2.contains(234));
}
//---------------------------------------------------------------------------
TEST(TestChainigHashTable, IteratorConcept) {
    ASSERT_TRUE(forward_iterator<ChainingHashTable::iterator>);
}
//---------------------------------------------------------------------------
TEST(TestChainingHashTable, Iterators) {
    ChainingHashTable ht;
    ChainingHashTable::iterator it;
    EXPECT_EQ(ht.begin(), ht.end());
    ht.insert(123, GV(456));
    EXPECT_NE(ht.begin(), ht.end());
    it = ht.begin();
    EXPECT_EQ((*it).key, 123);
    EXPECT_EQ(asInt(it->value), 456);
    auto it2 = it;
    EXPECT_EQ((*it2).key, 123);
    EXPECT_EQ(asInt(it2->value), 456);
    ++it;
    EXPECT_EQ(it, ht.end());
}
//---------------------------------------------------------------------------
TEST(TestChainingHashTable, IteratorPreIncrement) {
    ChainingHashTable ht;
    for (int i = 0; i < 100; ++i) {
        ht.insert(i, GV(i + 100));
    }
    auto it = ht.begin();
    for (int i = 0; i < 100; ++i) {
        SCOPED_TRACE(i);
        auto key = it->key;
        EXPECT_EQ(asInt(it->value), key + 100);
        ++it;
    }
    EXPECT_EQ(it, ht.end());
}
//---------------------------------------------------------------------------
TEST(TestChainingHashTable, IteratorPostIncrement) {
    ChainingHashTable ht;
    for (int i = 0; i < 100; ++i) {
        ht.insert(i, GV(i + 100));
    }
    auto it = ht.begin();
    for (int i = 0; i < 100; ++i) {
        SCOPED_TRACE(i);
        auto key = it->key;
        EXPECT_EQ(asInt(it->value), key + 100);
        auto oldIt = it++;
        EXPECT_EQ(oldIt->key, key);
        EXPECT_EQ(asInt(oldIt->value), key + 100);
    }
    EXPECT_EQ(it, ht.end());
}
//---------------------------------------------------------------------------
TEST(TestChainingHashTable, Find) {
    ChainingHashTable ht;
    EXPECT_EQ(ht.find(123), ht.end());
    for (int i = 0; i < 100; ++i) {
        ht.insert(i, GV(i + 100));
    }
    EXPECT_EQ(ht.find(123), ht.end());
    for (int i = 0; i < 100; ++i) {
        SCOPED_TRACE(i);
        auto it = ht.find(i);
        EXPECT_EQ(it->key, i);
        EXPECT_EQ(asInt(it->value), i + 100);
    }
    for (int i = 0; i < 100; ++i) {
        SCOPED_TRACE(i);
        ht.erase(i);
        EXPECT_EQ(ht.find(i), ht.end());
    }
}
//---------------------------------------------------------------------------
TEST(TestChainingHashTable, DestructorIsNotRecursive) {
    unique_ptr<ChainingHashTable> ht = make_unique<ChainingHashTable>();

    for (int64_t i = 0; i < 60000; ++i) {
        int64_t value = i << 18;
        ht->insert(value, GV(i));
    }

    // Explicitly destroy the ht object
    ht.reset();
}
//---------------------------------------------------------------------------

