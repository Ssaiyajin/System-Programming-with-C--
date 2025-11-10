#include "lib/BinaryHeap.hpp"
#include <gtest/gtest.h>
//---------------------------------------------------------------------------
using namespace std;
using namespace binary_heap;
//---------------------------------------------------------------------------
static const char* expectedOutput =
    "digraph {\n"
    "\t0 [label=\"20\"];\n"
    "\t1 [label=\"18\"];\n"
    "\t2 [label=\"8\"];\n"
    "\t3 [label=\"14\"];\n"
    "\t4 [label=\"16\"];\n"
    "\t5 [label=\"2\"];\n"
    "\t6 [label=\"4\"];\n"
    "\t7 [label=\"12\"];\n"
    "\t8 [label=\"6\"];\n"
    "\t9 [label=\"10\"];\n"
    "\t0 -> 1;\n"
    "\t1 -> 3;\n"
    "\t3 -> 7;\n"
    "\t3 -> 8;\n"
    "\t1 -> 4;\n"
    "\t4 -> 9;\n"
    "\t0 -> 2;\n"
    "\t2 -> 5;\n"
    "\t2 -> 6;\n"
    "}\n";
//---------------------------------------------------------------------------
TEST(TestBinaryHeap, insert) {
    vector<unsigned> data = {14, 12, 8, 20, 10, 2, 4, 18, 6, 16};

    vector<unsigned> heap;

    insert(heap, data[0]);
    ASSERT_EQ(heap, (vector<unsigned>{14}));
    insert(heap, data[1]);
    ASSERT_EQ(heap, (vector<unsigned>{14, 12}));
    insert(heap, data[2]);
    ASSERT_EQ(heap, (vector<unsigned>{14, 12, 8}));
    insert(heap, data[3]);
    ASSERT_EQ(heap, (vector<unsigned>{20, 14, 8, 12}));
    insert(heap, data[4]);
    ASSERT_EQ(heap, (vector<unsigned>{20, 14, 8, 12, 10}));
    insert(heap, data[5]);
    ASSERT_EQ(heap, (vector<unsigned>{20, 14, 8, 12, 10, 2}));
    insert(heap, data[6]);
    ASSERT_EQ(heap, (vector<unsigned>{20, 14, 8, 12, 10, 2, 4}));
    insert(heap, data[7]);
    ASSERT_EQ(heap, (vector<unsigned>{20, 18, 8, 14, 10, 2, 4, 12}));
    insert(heap, data[8]);
    ASSERT_EQ(heap, (vector<unsigned>{20, 18, 8, 14, 10, 2, 4, 12, 6}));
    insert(heap, data[9]);
    ASSERT_EQ(heap, (vector<unsigned>{20, 18, 8, 14, 16, 2, 4, 12, 6, 10}));
}
//---------------------------------------------------------------------------
TEST(TestBinaryHeap, extract) {
    vector<unsigned> data = {14, 12, 8, 20, 10, 2, 4, 18, 6, 16};

    vector<unsigned> heap;
    for (const auto& e : data)
        insert(heap, e);

    ASSERT_EQ(extract(heap), 20);
    ASSERT_EQ(heap, (vector<unsigned>{18, 16, 8, 14, 10, 2, 4, 12, 6}));
    ASSERT_EQ(extract(heap), 18);
    ASSERT_EQ(heap, (vector<unsigned>{16, 14, 8, 12, 10, 2, 4, 6}));
    ASSERT_EQ(extract(heap), 16);
    ASSERT_EQ(heap, (vector<unsigned>{14, 12, 8, 6, 10, 2, 4}));
    ASSERT_EQ(extract(heap), 14);
    ASSERT_EQ(heap, (vector<unsigned>{12, 10, 8, 6, 4, 2}));
    ASSERT_EQ(extract(heap), 12);
    ASSERT_EQ(heap, (vector<unsigned>{10, 6, 8, 2, 4}));
    ASSERT_EQ(extract(heap), 10);
    ASSERT_EQ(heap, (vector<unsigned>{8, 6, 4, 2}));
    ASSERT_EQ(extract(heap), 8);
    ASSERT_EQ(heap, (vector<unsigned>{6, 2, 4}));
    ASSERT_EQ(extract(heap), 6);
    ASSERT_EQ(heap, (vector<unsigned>{4, 2}));
    ASSERT_EQ(extract(heap), 4);
    ASSERT_EQ(heap, (vector<unsigned>{2}));
    ASSERT_EQ(extract(heap), 2);
    ASSERT_TRUE(heap.empty());
}
//---------------------------------------------------------------------------
TEST(TestBinaryHeap, printDot) {
    vector<unsigned> data = {14, 12, 8, 20, 10, 2, 4, 18, 6, 16};

    vector<unsigned> heap;
    for (const auto& e : data)
        insert(heap, e);

    ostringstream buf;
    printDot(buf, heap);

    ASSERT_EQ(buf.str(), expectedOutput);
}
//---------------------------------------------------------------------------