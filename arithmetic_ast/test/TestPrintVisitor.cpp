#include "lib/AST.hpp"
#include "lib/PrintVisitor.hpp"
#include <iostream>
#include <memory>
#include <sstream>
#include <utility>
#include <gtest/gtest.h>
//---------------------------------------------------------------------------
using namespace ast;
using namespace std;
//---------------------------------------------------------------------------
namespace {
//---------------------------------------------------------------------------
class CaptureCout {
    private:
    streambuf* sbuf;

    public:
    stringstream stream;

    CaptureCout() : sbuf(cout.rdbuf()) {
        cout.rdbuf(stream.rdbuf());
    }

    ~CaptureCout() {
        cout.rdbuf(sbuf);
    }
};
//---------------------------------------------------------------------------
} // namespace
//---------------------------------------------------------------------------
TEST(TestPrintVisitor, Constant) {
    CaptureCout cout;
    unique_ptr<ASTNode> node = make_unique<Constant>(1);
    PrintVisitor visitor;
    node->accept(visitor);
    EXPECT_EQ(cout.stream.str(), "1");
}
//---------------------------------------------------------------------------
TEST(TestPrintVisitor, Parameter) {
    CaptureCout cout;
    unique_ptr<ASTNode> node = make_unique<Parameter>(42);
    PrintVisitor visitor;
    node->accept(visitor);
    EXPECT_EQ(cout.stream.str(), "P42");
}
//---------------------------------------------------------------------------
TEST(TestPrintVisitor, UnaryPlus) {
    CaptureCout cout;
    unique_ptr<ASTNode> node = make_unique<UnaryPlus>(make_unique<Parameter>(42));
    PrintVisitor visitor;
    node->accept(visitor);
    EXPECT_EQ(cout.stream.str(), "(+P42)");
}
//---------------------------------------------------------------------------
TEST(TestPrintVisitor, UnaryMinus) {
    CaptureCout cout;
    unique_ptr<ASTNode> node = make_unique<UnaryMinus>(make_unique<Parameter>(42));
    PrintVisitor visitor;
    node->accept(visitor);
    EXPECT_EQ(cout.stream.str(), "(-P42)");
}
//---------------------------------------------------------------------------
TEST(TestPrintVisitor, Add) {
    CaptureCout cout;
    auto p1 = make_unique<Parameter>(1);
    auto p42 = make_unique<Parameter>(42);
    unique_ptr<ASTNode> node = make_unique<Add>(move(p1), move(p42));
    PrintVisitor visitor;
    node->accept(visitor);
    EXPECT_EQ(cout.stream.str(), "(P1 + P42)");
}
//---------------------------------------------------------------------------
TEST(TestPrintVisitor, Subtract) {
    CaptureCout cout;
    auto p1 = make_unique<Parameter>(1);
    auto p42 = make_unique<Parameter>(42);
    unique_ptr<ASTNode> node = make_unique<Subtract>(move(p1), move(p42));
    PrintVisitor visitor;
    node->accept(visitor);
    EXPECT_EQ(cout.stream.str(), "(P1 - P42)");
}
//---------------------------------------------------------------------------
TEST(TestPrintVisitor, Multiply) {
    CaptureCout cout;
    auto p1 = make_unique<Parameter>(1);
    auto p42 = make_unique<Parameter>(42);
    unique_ptr<ASTNode> node = make_unique<Multiply>(move(p1), move(p42));
    PrintVisitor visitor;
    node->accept(visitor);
    EXPECT_EQ(cout.stream.str(), "(P1 * P42)");
}
//---------------------------------------------------------------------------
TEST(TestPrintVisitor, Divide) {
    CaptureCout cout;
    auto p1 = make_unique<Parameter>(1);
    auto p42 = make_unique<Parameter>(42);
    unique_ptr<ASTNode> node = make_unique<Divide>(move(p1), move(p42));
    PrintVisitor visitor;
    node->accept(visitor);
    EXPECT_EQ(cout.stream.str(), "(P1 / P42)");
}
//---------------------------------------------------------------------------
TEST(TestPrintVisitor, Power) {
    CaptureCout cout;
    auto p1 = make_unique<Parameter>(1);
    auto p42 = make_unique<Parameter>(42);
    unique_ptr<ASTNode> node = make_unique<Power>(move(p1), move(p42));
    PrintVisitor visitor;
    node->accept(visitor);
    EXPECT_EQ(cout.stream.str(), "(P1 ^ P42)");
}
//---------------------------------------------------------------------------
TEST(TestPrintVisitor, Nested) {
    CaptureCout cout;
    auto p0 = make_unique<Parameter>(0);
    auto p1 = make_unique<Parameter>(1);
    auto p2 = make_unique<Parameter>(2);
    auto p3 = make_unique<Parameter>(3);
    unique_ptr<ASTNode> node1 = make_unique<Add>(move(p0), move(p1));
    unique_ptr<ASTNode> node2 = make_unique<Multiply>(move(p2), move(p3));
    unique_ptr<ASTNode> node = make_unique<Power>(move(node1), move(node2));
    node = make_unique<UnaryMinus>(move(node));
    PrintVisitor visitor;
    node->accept(visitor);
    EXPECT_EQ(cout.stream.str(), "(-((P0 + P1) ^ (P2 * P3)))");
}
//---------------------------------------------------------------------------
