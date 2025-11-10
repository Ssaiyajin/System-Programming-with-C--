#include "lib/AST.hpp"
#include "lib/EvaluationContext.hpp"
#include <cmath>
#include <utility>
#include <gtest/gtest.h>
//---------------------------------------------------------------------------
using namespace ast;
using namespace std;
//---------------------------------------------------------------------------
TEST(TestAST, EvaluateConstant) {
    EvaluationContext context;
    Constant c(1.0);
    ASSERT_EQ(c.evaluate(context), 1.0);
}
//---------------------------------------------------------------------------
TEST(TestAST, EvaluateUnaryPlus) {
    EvaluationContext context;
    auto node = make_unique<UnaryPlus>(make_unique<Constant>(1.0));
    EXPECT_EQ(node->evaluate(context), 1.0);
}
//---------------------------------------------------------------------------
TEST(TestAST, EvaluateUnaryMinus) {
    EvaluationContext context;
    auto node = make_unique<UnaryMinus>(make_unique<Constant>(1.0));
    EXPECT_EQ(node->evaluate(context), -1.0);
}
//---------------------------------------------------------------------------
TEST(TestAST, EvaluateAdd) {
    EvaluationContext context;
    auto node = make_unique<Add>(make_unique<Constant>(1.0), make_unique<Constant>(2.0));
    EXPECT_EQ(node->evaluate(context), 3.0);
}
//---------------------------------------------------------------------------
TEST(TestAST, EvaluateSubtract) {
    EvaluationContext context;
    auto node = make_unique<Subtract>(make_unique<Constant>(1.0), make_unique<Constant>(2.0));
    EXPECT_EQ(node->evaluate(context), -1.0);
}
//---------------------------------------------------------------------------
TEST(TestAST, EvaluateMultiply) {
    EvaluationContext context;
    auto node = make_unique<Multiply>(make_unique<Constant>(2.0), make_unique<Constant>(3.0));
    EXPECT_EQ(node->evaluate(context), 6.0);
}
//---------------------------------------------------------------------------
TEST(TestAST, EvaluateDivide) {
    EvaluationContext context;
    auto node = make_unique<Divide>(make_unique<Constant>(1.0), make_unique<Constant>(2.0));
    EXPECT_EQ(node->evaluate(context), 0.5);
}
//---------------------------------------------------------------------------
TEST(TestAST, EvaluatePower) {
    EvaluationContext context;
    auto node = make_unique<Power>(make_unique<Constant>(2.0), make_unique<Constant>(3.0));
    EXPECT_EQ(node->evaluate(context), 8.0);
}
//---------------------------------------------------------------------------
TEST(TestAST, EvaluateParameter) {
    EvaluationContext context;
    context.pushParameter(1.0);
    auto node = make_unique<Parameter>(0);
    EXPECT_EQ(node->evaluate(context), 1.0);
}
//---------------------------------------------------------------------------
TEST(TestAST, EvaluateNested) {
    EvaluationContext context;
    context.pushParameter(2.0);
    context.pushParameter(4.0);
    unique_ptr<ASTNode> node1 = make_unique<Add>(make_unique<Parameter>(0), make_unique<Parameter>(1));
    unique_ptr<ASTNode> node2 = make_unique<Subtract>(make_unique<Parameter>(1), make_unique<Parameter>(0));
    unique_ptr<ASTNode> node = make_unique<Multiply>(move(node1), move(node2));
    node = make_unique<Divide>(move(node), make_unique<Constant>(2.0));
    node = make_unique<Power>(move(node), make_unique<Constant>(2.0));
    EXPECT_EQ(node->evaluate(context), 36.0);
}
//---------------------------------------------------------------------------
TEST(TestAST, OptimizeUnaryPlus) {
    SCOPED_TRACE("+a -> a");
    auto a = make_unique<Parameter>(0);
    auto* aPtr = a.get();
    unique_ptr<ASTNode> node = make_unique<UnaryPlus>(move(a));
    node->optimize(node);
    EXPECT_EQ(node.get(), aPtr);
}
//---------------------------------------------------------------------------
TEST(TestAST, OptimizeUnaryMinusConstant) {
    unique_ptr<ASTNode> node = make_unique<UnaryMinus>(make_unique<Constant>(1.0));
    node->optimize(node);
    ASSERT_EQ(node->getType(), ASTNode::Type::Constant);
    auto& c = static_cast<Constant&>(*node);
    EXPECT_EQ(c.getValue(), -1.0);
}
//---------------------------------------------------------------------------
TEST(TestAST, OptimizeUnaryMinus1) {
    SCOPED_TRACE("-(-a) -> a");
    auto a = make_unique<Parameter>(0);
    auto* aPtr = a.get();
    unique_ptr<ASTNode> node = make_unique<UnaryMinus>(move(a));
    node = make_unique<UnaryMinus>(move(node));
    node->optimize(node);
    EXPECT_EQ(node.get(), aPtr);
}
//---------------------------------------------------------------------------
TEST(TestAST, OptimizeUnaryMinus2) {
    SCOPED_TRACE("-(a - b) -> b - a");
    auto a = make_unique<Parameter>(0);
    auto* aPtr = a.get();
    auto b = make_unique<Parameter>(1);
    auto* bPtr = b.get();
    unique_ptr<ASTNode> node = make_unique<Subtract>(move(a), move(b));
    node = make_unique<UnaryMinus>(move(node));
    node->optimize(node);
    ASSERT_EQ(node->getType(), ASTNode::Type::Subtract);
    auto& s = static_cast<Subtract&>(*node);
    const auto& input0 = s.getLeft();
    const auto& input1 = s.getRight();
    EXPECT_EQ(&input0, bPtr);
    EXPECT_EQ(&input1, aPtr);
}
//---------------------------------------------------------------------------
TEST(TestAST, OptimizeUnaryMinus3) {
    SCOPED_TRACE("-((-a) - b) -> b + a");
    auto a = make_unique<Parameter>(0);
    auto* aPtr = a.get();
    auto b = make_unique<Parameter>(1);
    auto* bPtr = b.get();
    unique_ptr<ASTNode> node = make_unique<UnaryMinus>(move(a));
    node = make_unique<Subtract>(move(node), move(b));
    node = make_unique<UnaryMinus>(move(node));
    node->optimize(node);
    ASSERT_EQ(node->getType(), ASTNode::Type::Add);
    auto& s = static_cast<Add&>(*node);
    const auto& input0 = s.getLeft();
    const auto& input1 = s.getRight();
    EXPECT_EQ(&input0, bPtr);
    EXPECT_EQ(&input1, aPtr);
}
//---------------------------------------------------------------------------
TEST(TestAST, OptimizeAddConstant) {
    unique_ptr<ASTNode> node = make_unique<Add>(make_unique<Constant>(1.0), make_unique<Constant>(2.0));
    node->optimize(node);
    ASSERT_EQ(node->getType(), ASTNode::Type::Constant);
    auto& c = static_cast<Constant&>(*node);
    EXPECT_EQ(c.getValue(), 3.0);
}
//---------------------------------------------------------------------------
TEST(TestAST, OptimizeAdd1) {
    SCOPED_TRACE("a + 0 -> a");
    auto a = make_unique<Parameter>(0);
    auto* aPtr = a.get();
    unique_ptr<ASTNode> node = make_unique<Add>(move(a), make_unique<Constant>(0));
    node->optimize(node);
    EXPECT_EQ(node.get(), aPtr);
}
//---------------------------------------------------------------------------
TEST(TestAST, OptimizeAdd2) {
    SCOPED_TRACE("0 + a -> a");
    auto a = make_unique<Parameter>(0);
    auto* aPtr = a.get();
    unique_ptr<ASTNode> node = make_unique<Add>(make_unique<Constant>(0), move(a));
    node->optimize(node);
    EXPECT_EQ(node.get(), aPtr);
}
//---------------------------------------------------------------------------
TEST(TestAST, OptimizeAdd3) {
    SCOPED_TRACE("(-a) + b -> b - a");
    auto a = make_unique<Parameter>(0);
    auto* aPtr = a.get();
    auto b = make_unique<Parameter>(1);
    auto* bPtr = b.get();
    unique_ptr<ASTNode> node = make_unique<UnaryMinus>(move(a));
    node = make_unique<Add>(move(node), move(b));
    node->optimize(node);
    ASSERT_EQ(node->getType(), ASTNode::Type::Subtract);
    auto& s = static_cast<Subtract&>(*node);
    const auto& input0 = s.getLeft();
    const auto& input1 = s.getRight();
    EXPECT_EQ(&input0, bPtr);
    EXPECT_EQ(&input1, aPtr);
}
//---------------------------------------------------------------------------
TEST(TestAST, OptimizeAdd4) {
    SCOPED_TRACE("a + (-b) -> a - b");
    auto a = make_unique<Parameter>(0);
    auto* aPtr = a.get();
    auto b = make_unique<Parameter>(1);
    auto* bPtr = b.get();
    unique_ptr<ASTNode> node = make_unique<UnaryMinus>(move(b));
    node = make_unique<Add>(move(a), move(node));
    node->optimize(node);
    ASSERT_EQ(node->getType(), ASTNode::Type::Subtract);
    auto& s = static_cast<Subtract&>(*node);
    const auto& input0 = s.getLeft();
    const auto& input1 = s.getRight();
    EXPECT_EQ(&input0, aPtr);
    EXPECT_EQ(&input1, bPtr);
}
//---------------------------------------------------------------------------
TEST(TestAST, OptimizeSubtractConstant) {
    unique_ptr<ASTNode> node = make_unique<Subtract>(make_unique<Constant>(1.0), make_unique<Constant>(2.0));
    node->optimize(node);
    ASSERT_EQ(node->getType(), ASTNode::Type::Constant);
    auto& c = static_cast<Constant&>(*node);
    EXPECT_EQ(c.getValue(), -1.0);
}
//---------------------------------------------------------------------------
TEST(TestAST, OptimizeSubtract1) {
    SCOPED_TRACE("a - 0 -> a");
    auto a = make_unique<Parameter>(0);
    auto* aPtr = a.get();
    unique_ptr<ASTNode> node = make_unique<Subtract>(move(a), make_unique<Constant>(0));
    node->optimize(node);
    EXPECT_EQ(node.get(), aPtr);
}
//---------------------------------------------------------------------------
TEST(TestAST, OptimizeSubtract2) {
    SCOPED_TRACE("0 - a -> -a");
    auto a = make_unique<Parameter>(0);
    auto* aPtr = a.get();
    unique_ptr<ASTNode> node = make_unique<Subtract>(make_unique<Constant>(0), move(a));
    node->optimize(node);
    ASSERT_EQ(node->getType(), ASTNode::Type::UnaryMinus);
    auto& u = static_cast<UnaryMinus&>(*node);
    const auto& input0 = u.getInput();
    EXPECT_EQ(&input0, aPtr);
}
//---------------------------------------------------------------------------
TEST(TestAST, OptimizeSubtract3) {
    SCOPED_TRACE("a - (-b) -> a + b");
    auto a = make_unique<Parameter>(0);
    auto* aPtr = a.get();
    auto b = make_unique<Parameter>(1);
    auto* bPtr = b.get();
    unique_ptr<ASTNode> node = make_unique<UnaryMinus>(move(b));
    node = make_unique<Subtract>(move(a), move(node));
    node->optimize(node);
    ASSERT_EQ(node->getType(), ASTNode::Type::Add);
    auto& add = static_cast<Add&>(*node);
    const auto& input0 = add.getLeft();
    const auto& input1 = add.getRight();
    EXPECT_EQ(&input0, aPtr);
    EXPECT_EQ(&input1, bPtr);
}
//---------------------------------------------------------------------------
TEST(TestAST, OptimizeMultiplyConstant) {
    unique_ptr<ASTNode> node = make_unique<Multiply>(make_unique<Constant>(2.0), make_unique<Constant>(3.0));
    node->optimize(node);
    ASSERT_EQ(node->getType(), ASTNode::Type::Constant);
    auto& c = static_cast<Constant&>(*node);
    EXPECT_EQ(c.getValue(), 6.0);
}
//---------------------------------------------------------------------------
TEST(TestAST, OptimizeMultiply1) {
    SCOPED_TRACE("a * 0 -> 0");
    auto a = make_unique<Parameter>(0);
    unique_ptr<ASTNode> node = make_unique<Multiply>(move(a), make_unique<Constant>(0));
    node->optimize(node);
    ASSERT_EQ(node->getType(), ASTNode::Type::Constant);
    auto& c = static_cast<Constant&>(*node);
    EXPECT_EQ(c.getValue(), 0.0);
}
//---------------------------------------------------------------------------
TEST(TestAST, OptimizeMultiply2) {
    SCOPED_TRACE("a * 1 -> a");
    auto a = make_unique<Parameter>(0);
    auto* aPtr = a.get();
    unique_ptr<ASTNode> node = make_unique<Multiply>(move(a), make_unique<Constant>(1));
    node->optimize(node);
    EXPECT_EQ(node.get(), aPtr);
}
//---------------------------------------------------------------------------
TEST(TestAST, OptimizeMultiply3) {
    SCOPED_TRACE("0 * a -> 0");
    auto a = make_unique<Parameter>(0);
    unique_ptr<ASTNode> node = make_unique<Multiply>(make_unique<Constant>(0), move(a));
    node->optimize(node);
    ASSERT_EQ(node->getType(), ASTNode::Type::Constant);
    auto& c = static_cast<Constant&>(*node);
    EXPECT_EQ(c.getValue(), 0.0);
}
//---------------------------------------------------------------------------
TEST(TestAST, OptimizeMultiply4) {
    SCOPED_TRACE("1 * a -> a");
    auto a = make_unique<Parameter>(0);
    auto* aPtr = a.get();
    unique_ptr<ASTNode> node = make_unique<Multiply>(make_unique<Constant>(1), move(a));
    node->optimize(node);
    EXPECT_EQ(node.get(), aPtr);
}
//---------------------------------------------------------------------------
TEST(TestAST, OptimizeMultiply5) {
    SCOPED_TRACE("(-a) * (-b) -> a * b");
    auto a = make_unique<Parameter>(0);
    auto* aPtr = a.get();
    auto b = make_unique<Parameter>(1);
    auto* bPtr = b.get();
    unique_ptr<ASTNode> aMinus = make_unique<UnaryMinus>(move(a));
    unique_ptr<ASTNode> bMinus = make_unique<UnaryMinus>(move(b));
    unique_ptr<ASTNode> node = make_unique<Multiply>(move(aMinus), move(bMinus));
    node->optimize(node);
    ASSERT_EQ(node->getType(), ASTNode::Type::Multiply);
    auto& m = static_cast<Multiply&>(*node);
    const auto& input0 = m.getLeft();
    const auto& input1 = m.getRight();
    EXPECT_EQ(&input0, aPtr);
    EXPECT_EQ(&input1, bPtr);
}
//---------------------------------------------------------------------------
TEST(TestAST, OptimizeDivideConstant) {
    unique_ptr<ASTNode> node = make_unique<Divide>(make_unique<Constant>(1.0), make_unique<Constant>(2.0));
    node->optimize(node);
    ASSERT_EQ(node->getType(), ASTNode::Type::Constant);
    auto& c = static_cast<Constant&>(*node);
    EXPECT_EQ(c.getValue(), 0.5);
}
//---------------------------------------------------------------------------
TEST(TestAST, OptimizeDivide1) {
    SCOPED_TRACE("a / 1 -> a");
    auto a = make_unique<Parameter>(0);
    auto* aPtr = a.get();
    unique_ptr<ASTNode> node = make_unique<Divide>(move(a), make_unique<Constant>(1));
    node->optimize(node);
    EXPECT_EQ(node.get(), aPtr);
}
//---------------------------------------------------------------------------
TEST(TestAST, OptimizeDivide2) {
    SCOPED_TRACE("a / c -> a * (1 / c)");
    auto a = make_unique<Parameter>(0);
    auto* aPtr = a.get();
    unique_ptr<ASTNode> node = make_unique<Divide>(move(a), make_unique<Constant>(2));
    node->optimize(node);
    ASSERT_EQ(node->getType(), ASTNode::Type::Multiply);
    auto& m = static_cast<Multiply&>(*node);
    const auto& input0 = m.getLeft();
    const auto& input1 = m.getRight();
    EXPECT_EQ(&input0, aPtr);
    ASSERT_EQ(input1.getType(), ASTNode::Type::Constant);
    const auto& c = static_cast<const Constant&>(input1);
    EXPECT_EQ(c.getValue(), 0.5);
}
//---------------------------------------------------------------------------
TEST(TestAST, OptimizeDivide3) {
    SCOPED_TRACE("0 / a -> 0");
    auto a = make_unique<Parameter>(0);
    unique_ptr<ASTNode> node = make_unique<Divide>(make_unique<Constant>(0), move(a));
    node->optimize(node);
    ASSERT_EQ(node->getType(), ASTNode::Type::Constant);
    auto& c = static_cast<Constant&>(*node);
    EXPECT_EQ(c.getValue(), 0.0);
}
//---------------------------------------------------------------------------
TEST(TestAST, OptimizeDivide4) {
    SCOPED_TRACE("(-a) / (-b) -> a / b");
    auto a = make_unique<Parameter>(0);
    auto* aPtr = a.get();
    auto b = make_unique<Parameter>(1);
    auto* bPtr = b.get();
    unique_ptr<ASTNode> aMinus = make_unique<UnaryMinus>(move(a));
    unique_ptr<ASTNode> bMinus = make_unique<UnaryMinus>(move(b));
    unique_ptr<ASTNode> node = make_unique<Divide>(move(aMinus), move(bMinus));
    node->optimize(node);
    ASSERT_EQ(node->getType(), ASTNode::Type::Divide);
    auto& d = static_cast<Divide&>(*node);
    const auto& input0 = d.getLeft();
    const auto& input1 = d.getRight();
    EXPECT_EQ(&input0, aPtr);
    EXPECT_EQ(&input1, bPtr);
}
//---------------------------------------------------------------------------
TEST(TestAST, OptimizePowerConstant) {
    unique_ptr<ASTNode> node = make_unique<Power>(make_unique<Constant>(2.0), make_unique<Constant>(3.0));
    node->optimize(node);
    ASSERT_EQ(node->getType(), ASTNode::Type::Constant);
    auto& c = static_cast<Constant&>(*node);
    EXPECT_EQ(c.getValue(), 8.0);
}
//---------------------------------------------------------------------------
TEST(TestAST, OptimizePower1) {
    SCOPED_TRACE("a ^ 0 -> 1");
    auto a = make_unique<Parameter>(0);
    unique_ptr<ASTNode> node = make_unique<Power>(move(a), make_unique<Constant>(0));
    node->optimize(node);
    ASSERT_EQ(node->getType(), ASTNode::Type::Constant);
    auto& c = static_cast<Constant&>(*node);
    EXPECT_EQ(c.getValue(), 1.0);
}
//---------------------------------------------------------------------------
TEST(TestAST, OptimizePower2) {
    SCOPED_TRACE("a ^ 1 -> a");
    auto a = make_unique<Parameter>(0);
    auto* aPtr = a.get();
    unique_ptr<ASTNode> node = make_unique<Power>(move(a), make_unique<Constant>(1));
    node->optimize(node);
    EXPECT_EQ(node.get(), aPtr);
}
//---------------------------------------------------------------------------
TEST(TestAST, OptimizePower3) {
    SCOPED_TRACE("a ^ -1 -> 1 / a");
    auto a = make_unique<Parameter>(0);
    auto* aPtr = a.get();
    unique_ptr<ASTNode> node = make_unique<Power>(move(a), make_unique<Constant>(-1));
    node->optimize(node);
    ASSERT_EQ(node->getType(), ASTNode::Type::Divide);
    auto& d = static_cast<Divide&>(*node);
    const auto& input0 = d.getLeft();
    const auto& input1 = d.getRight();
    ASSERT_EQ(input0.getType(), ASTNode::Type::Constant);
    const auto& c = static_cast<const Constant&>(input0);
    EXPECT_EQ(c.getValue(), 1.0);
    EXPECT_EQ(&input1, aPtr);
}
//---------------------------------------------------------------------------
TEST(TestAST, OptimizePower4) {
    SCOPED_TRACE("0 ^ a -> 0");
    auto a = make_unique<Parameter>(0);
    unique_ptr<ASTNode> node = make_unique<Power>(make_unique<Constant>(0), move(a));
    node->optimize(node);
    ASSERT_EQ(node->getType(), ASTNode::Type::Constant);
    auto& c = static_cast<Constant&>(*node);
    EXPECT_EQ(c.getValue(), 0.0);
}
//---------------------------------------------------------------------------
TEST(TestAST, OptimizePower5) {
    SCOPED_TRACE("1 ^ a -> 1");
    auto a = make_unique<Parameter>(0);
    unique_ptr<ASTNode> node = make_unique<Power>(make_unique<Constant>(1), move(a));
    node->optimize(node);
    ASSERT_EQ(node->getType(), ASTNode::Type::Constant);
    auto& c = static_cast<Constant&>(*node);
    EXPECT_EQ(c.getValue(), 1.0);
}
//---------------------------------------------------------------------------
TEST(TestAST, OptimizeNested1) {
    SCOPED_TRACE("(2 + 3) ^ 2 - 3 * 3 -> 16");
    unique_ptr<ASTNode> node = make_unique<Add>(make_unique<Constant>(2), make_unique<Constant>(3));
    node = make_unique<Power>(move(node), make_unique<Constant>(2));
    node = make_unique<Subtract>(move(node), make_unique<Multiply>(make_unique<Constant>(3), make_unique<Constant>(3)));
    node->optimize(node);
    ASSERT_EQ(node->getType(), ASTNode::Type::Constant);
    auto& c = static_cast<Constant&>(*node);
    EXPECT_EQ(c.getValue(), 16.0);
}
//---------------------------------------------------------------------------
TEST(TestAST, OptimizeNested2) {
    SCOPED_TRACE("-(-a) + b/1 -> a + b");
    auto a = make_unique<Parameter>(0);
    auto* aPtr = a.get();
    auto b = make_unique<Parameter>(1);
    auto* bPtr = b.get();
    unique_ptr<ASTNode> node1 = make_unique<UnaryMinus>(make_unique<UnaryMinus>(move(a)));
    unique_ptr<ASTNode> node2 = make_unique<Divide>(move(b), make_unique<Constant>(1));
    unique_ptr<ASTNode> node = make_unique<Add>(move(node1), move(node2));
    node->optimize(node);
    ASSERT_EQ(node->getType(), ASTNode::Type::Add);
    auto& add = static_cast<Add&>(*node);
    const auto& input0 = add.getLeft();
    const auto& input1 = add.getRight();
    EXPECT_EQ(&input0, aPtr);
    EXPECT_EQ(&input1, bPtr);
}
//---------------------------------------------------------------------------
TEST(TestAST, OptimizeNested3) {
    SCOPED_TRACE("a^1 + 1 * -b -> a - b");
    auto a = make_unique<Parameter>(0);
    auto* aPtr = a.get();
    auto b = make_unique<Parameter>(1);
    auto* bPtr = b.get();
    unique_ptr<ASTNode> node1 = make_unique<Power>(move(a), make_unique<Constant>(1));
    unique_ptr<ASTNode> node2 = make_unique<Multiply>(make_unique<Constant>(1), make_unique<UnaryMinus>(move(b)));
    unique_ptr<ASTNode> node = make_unique<Add>(move(node1), move(node2));
    node->optimize(node);
    ASSERT_EQ(node->getType(), ASTNode::Type::Subtract);
    auto& s = static_cast<Subtract&>(*node);
    const auto& input0 = s.getLeft();
    const auto& input1 = s.getRight();
    EXPECT_EQ(&input0, aPtr);
    EXPECT_EQ(&input1, bPtr);
}
//---------------------------------------------------------------------------
