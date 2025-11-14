#include "lib/AST.hpp"
#include "lib/EvaluationContext.hpp"
#include "lib/ASTVisitor.hpp"
#include <cmath>
#include <type_traits>

namespace ast {

// --- Constant -------------------------------------------------------------
Constant::Constant(double v) : value(v) {}
ASTNode::Type Constant::getType() const { return ASTNode::Type::Constant; }
double Constant::evaluate(EvaluationContext& ctx) { (void)ctx; return value; }
void Constant::accept(ASTVisitor& visitor) { visitor.visit(*this); }
void Constant::optimize(std::unique_ptr<ASTNode>& thisRef) { (void)thisRef; }
double Constant::getValue() const { return value; }

// --- Parameter ------------------------------------------------------------
Parameter::Parameter(size_t idx) : m_index(idx) {}
ASTNode::Type Parameter::getType() const { return ASTNode::Type::Parameter; }
double Parameter::evaluate(EvaluationContext& ctx) { return ctx.getParameter(m_index); }
void Parameter::accept(ASTVisitor& visitor) { visitor.visit(*this); }
void Parameter::optimize(std::unique_ptr<ASTNode>& thisRef) { (void)thisRef; }
size_t Parameter::getIndex() const { return m_index; }

// --- UnaryPlus -----------------------------------------------------------
UnaryPlus::UnaryPlus(std::unique_ptr<ASTNode> child) : childNode(std::move(child)) {}
ASTNode::Type UnaryPlus::getType() const { return ASTNode::Type::UnaryPlus; }
double UnaryPlus::evaluate(EvaluationContext& ctx) { return childNode->evaluate(ctx); }
void UnaryPlus::accept(ASTVisitor& visitor) { visitor.visit(*this); }
std::unique_ptr<ASTNode>& UnaryPlus::getMutableInput() { return childNode; }
ASTNode& UnaryPlus::getInput() { return *childNode; }
const ASTNode& UnaryPlus::getInput() const { return *childNode; }
void UnaryPlus::optimize(std::unique_ptr<ASTNode>& thisRef) {
    if (!childNode) return;

    // Flatten +(+x) -> +x and +const/param -> child (preserve pointer identity)
    if (childNode->getType() == ASTNode::Type::UnaryPlus
        || childNode->getType() == ASTNode::Type::Constant
        || childNode->getType() == ASTNode::Type::Parameter) {
        thisRef = std::move(childNode);
        return;
    }

    // Otherwise optimize child in-place
    childNode->optimize(childNode);
}

// --- UnaryMinus ----------------------------------------------------------
UnaryMinus::UnaryMinus(std::unique_ptr<ASTNode> child) : childNode(std::move(child)) {}
ASTNode::Type UnaryMinus::getType() const { return ASTNode::Type::UnaryMinus; }
double UnaryMinus::evaluate(EvaluationContext& ctx) { return -childNode->evaluate(ctx); }
void UnaryMinus::accept(ASTVisitor& visitor) { visitor.visit(*this); }
std::unique_ptr<ASTNode>& UnaryMinus::getMutableInput() { return childNode; }
ASTNode& UnaryMinus::getInput() { return *childNode; }
const ASTNode& UnaryMinus::getInput() const { return *childNode; }

void UnaryMinus::optimize(std::unique_ptr<ASTNode>& thisRef) {
    if (childNode) childNode->optimize(childNode);

    if (!childNode) return;

    // -const -> const(-v)
    if (childNode->getType() == ASTNode::Type::Constant) {
        auto c = static_cast<Constant*>(childNode.get());
        thisRef = std::make_unique<Constant>(-c->getValue());
        return;
    }

    // -(-x) -> x
    if (childNode->getType() == ASTNode::Type::UnaryMinus) {
        auto inner = std::move(static_cast<UnaryMinus*>(childNode.get())->getMutableInput());
        thisRef = std::move(inner);
        return;
    }

    // -( ( -a ) - b ) -> b + a
    if (childNode->getType() == ASTNode::Type::Subtract) {
        auto sub = static_cast<Subtract*>(childNode.get());
        auto L = sub->releaseLeft();   // owns left
        auto R = sub->releaseRight();  // owns right

        // if left was unary minus: (-a) - b  => b + a
        if (L && L->getType() == ASTNode::Type::UnaryMinus) {
            auto inner = std::move(static_cast<UnaryMinus*>(L.get())->getMutableInput());
            thisRef = std::make_unique<Add>(std::move(R), std::move(inner));
            return;
        }

        // default: -(a - b) -> b - a
        thisRef = std::make_unique<Subtract>(std::move(R), std::move(L));
        return;
    }

    // default: keep unary minus (child already optimized)
}

// --- BinaryASTNode -------------------------------------------------------
BinaryASTNode::BinaryASTNode(std::unique_ptr<ASTNode> l, std::unique_ptr<ASTNode> r)
    : leftNode(std::move(l)), rightNode(std::move(r)) {}

ASTNode& BinaryASTNode::getLeft() { return *leftNode; }
ASTNode& BinaryASTNode::getRight() { return *rightNode; }
std::unique_ptr<ASTNode> BinaryASTNode::releaseLeft() { return std::move(leftNode); }
std::unique_ptr<ASTNode> BinaryASTNode::releaseRight() { return std::move(rightNode); }
std::unique_ptr<ASTNode>& BinaryASTNode::getMutableLeft() { return leftNode; }
std::unique_ptr<ASTNode>& BinaryASTNode::getMutableRight() { return rightNode; }

// --- Add -----------------------------------------------------------------
Add::Add(std::unique_ptr<ASTNode> l, std::unique_ptr<ASTNode> r) : BinaryASTNode(std::move(l), std::move(r)) {}
ASTNode::Type Add::getType() const { return ASTNode::Type::Add; }
double Add::evaluate(EvaluationContext& ctx) { return getLeft().evaluate(ctx) + getRight().evaluate(ctx); }
void Add::accept(ASTVisitor& visitor) { visitor.visit(*this); }

void Add::optimize(std::unique_ptr<ASTNode>& thisRef) {
    if (getMutableLeft()) getMutableLeft()->optimize(getMutableLeft());
    if (getMutableRight()) getMutableRight()->optimize(getMutableRight());

    // both constants
    if (getLeft().getType() == ASTNode::Type::Constant && getRight().getType() == ASTNode::Type::Constant) {
        double a = static_cast<Constant*>(&getLeft())->getValue();
        double b = static_cast<Constant*>(&getRight())->getValue();
        thisRef = std::make_unique<Constant>(a + b);
        return;
    }

    // x + 0 -> x
    if (getRight().getType() == ASTNode::Type::Constant) {
        double b = static_cast<Constant*>(&getRight())->getValue();
        if (b == 0.0) { thisRef = releaseLeft(); return; }
    }
    if (getLeft().getType() == ASTNode::Type::Constant) {
        double a = static_cast<Constant*>(&getLeft())->getValue();
        if (a == 0.0) { thisRef = releaseRight(); return; }
    }

    // a + (-b) -> a - b
    if (getRight().getType() == ASTNode::Type::UnaryMinus) {
        auto um = static_cast<UnaryMinus*>(&getRight());
        auto inner = std::move(um->getMutableInput());
        auto L = releaseLeft();
        thisRef = std::make_unique<Subtract>(std::move(L), std::move(inner));
        return;
    }

    // (-a) + b -> b - a
    if (getLeft().getType() == ASTNode::Type::UnaryMinus) {
        auto um = static_cast<UnaryMinus*>(&getLeft());
        auto inner = std::move(um->getMutableInput());
        auto R = releaseRight();
        thisRef = std::make_unique<Subtract>(std::move(R), std::move(inner));
        return;
    }
}

// --- Subtract ------------------------------------------------------------
Subtract::Subtract(std::unique_ptr<ASTNode> l, std::unique_ptr<ASTNode> r) : BinaryASTNode(std::move(l), std::move(r)) {}
ASTNode::Type Subtract::getType() const { return ASTNode::Type::Subtract; }
double Subtract::evaluate(EvaluationContext& ctx) { return getLeft().evaluate(ctx) - getRight().evaluate(ctx); }
void Subtract::accept(ASTVisitor& visitor) { visitor.visit(*this); }

void Subtract::optimize(std::unique_ptr<ASTNode>& thisRef) {
    if (getMutableLeft()) getMutableLeft()->optimize(getMutableLeft());
    if (getMutableRight()) getMutableRight()->optimize(getMutableRight());

    if (getLeft().getType() == ASTNode::Type::Constant && getRight().getType() == ASTNode::Type::Constant) {
        double a = static_cast<Constant*>(&getLeft())->getValue();
        double b = static_cast<Constant*>(&getRight())->getValue();
        thisRef = std::make_unique<Constant>(a - b);
        return;
    }

    // 0 - x -> -x
    if (getLeft().getType() == ASTNode::Type::Constant) {
        double a = static_cast<Constant*>(&getLeft())->getValue();
        if (a == 0.0) { auto r = releaseRight(); thisRef = std::make_unique<UnaryMinus>(std::move(r)); return; }
    }

    // x - 0 -> x
    if (getRight().getType() == ASTNode::Type::Constant) {
        double b = static_cast<Constant*>(&getRight())->getValue();
        if (b == 0.0) { thisRef = releaseLeft(); return; }
    }

    // x - (-y) -> x + y
    if (getRight().getType() == ASTNode::Type::UnaryMinus) {
        auto um = static_cast<UnaryMinus*>(&getRight());
        auto inner = std::move(um->getMutableInput());
        auto L = releaseLeft();
        thisRef = std::make_unique<Add>(std::move(L), std::move(inner));
        return;
    }
}

// --- Multiply ------------------------------------------------------------
Multiply::Multiply(std::unique_ptr<ASTNode> l, std::unique_ptr<ASTNode> r) : BinaryASTNode(std::move(l), std::move(r)) {}
ASTNode::Type Multiply::getType() const { return ASTNode::Type::Multiply; }
double Multiply::evaluate(EvaluationContext& ctx) { return getLeft().evaluate(ctx) * getRight().evaluate(ctx); }
void Multiply::accept(ASTVisitor& visitor) { visitor.visit(*this); }

void Multiply::optimize(std::unique_ptr<ASTNode>& thisRef) {
    if (getMutableLeft()) getMutableLeft()->optimize(getMutableLeft());
    if (getMutableRight()) getMutableRight()->optimize(getMutableRight());

    if (getLeft().getType() == ASTNode::Type::Constant && getRight().getType() == ASTNode::Type::Constant) {
        double a = static_cast<Constant*>(&getLeft())->getValue();
        double b = static_cast<Constant*>(&getRight())->getValue();
        thisRef = std::make_unique<Constant>(a * b);
        return;
    }

    // 0 * x -> 0 ; 1 * x -> x
    if (getLeft().getType() == ASTNode::Type::Constant) {
        double a = static_cast<Constant*>(&getLeft())->getValue();
        if (a == 0.0) { thisRef = std::make_unique<Constant>(0.0); return; }
        if (a == 1.0) { thisRef = releaseRight(); return; }
    }
    if (getRight().getType() == ASTNode::Type::Constant) {
        double b = static_cast<Constant*>(&getRight())->getValue();
        if (b == 0.0) { thisRef = std::make_unique<Constant>(0.0); return; }
        if (b == 1.0) { thisRef = releaseLeft(); return; }
    }

    // (-a) * (-b) -> a * b
    if (getLeft().getType() == ASTNode::Type::UnaryMinus && getRight().getType() == ASTNode::Type::UnaryMinus) {
        auto Linner = std::move(static_cast<UnaryMinus*>(&getLeft())->getMutableInput());
        auto Rinner = std::move(static_cast<UnaryMinus*>(&getRight())->getMutableInput());
        thisRef = std::make_unique<Multiply>(std::move(Linner), std::move(Rinner));
        return;
    }
}

// --- Divide --------------------------------------------------------------
Divide::Divide(std::unique_ptr<ASTNode> l, std::unique_ptr<ASTNode> r) : BinaryASTNode(std::move(l), std::move(r)) {}
ASTNode::Type Divide::getType() const { return ASTNode::Type::Divide; }
double Divide::evaluate(EvaluationContext& ctx) {
    double rv = getRight().evaluate(ctx);
    if (rv == 0.0) return 0.0; // simplistic
    return getLeft().evaluate(ctx) / rv;
}
void Divide::accept(ASTVisitor& visitor) { visitor.visit(*this); }

void Divide::optimize(std::unique_ptr<ASTNode>& thisRef) {
    if (getMutableLeft()) getMutableLeft()->optimize(getMutableLeft());
    if (getMutableRight()) getMutableRight()->optimize(getMutableRight());

    // 0 / x -> 0 (even if x not known)
    if (getLeft().getType() == ASTNode::Type::Constant) {
        double a = static_cast<Constant*>(&getLeft())->getValue();
        if (a == 0.0) {
            thisRef = std::make_unique<Constant>(0.0);
            return;
        }
    }

    // both constants
    if (getLeft().getType() == ASTNode::Type::Constant && getRight().getType() == ASTNode::Type::Constant) {
        double a = static_cast<Constant*>(&getLeft())->getValue();
        double b = static_cast<Constant*>(&getRight())->getValue();
        if (b != 0.0) thisRef = std::make_unique<Constant>(a / b);
        else thisRef = std::make_unique<Constant>(0.0);
        return;
    }

    // x / 1 -> x
    if (getRight().getType() == ASTNode::Type::Constant) {
        double b = static_cast<Constant*>(&getRight())->getValue();
        if (b == 1.0) { thisRef = releaseLeft(); return; }
    }

    // (-a) / (-b) -> a / b
    if (getLeft().getType() == ASTNode::Type::UnaryMinus && getRight().getType() == ASTNode::Type::UnaryMinus) {
        auto Linner = std::move(static_cast<UnaryMinus*>(&getLeft())->getMutableInput());
        auto Rinner = std::move(static_cast<UnaryMinus*>(&getRight())->getMutableInput());
        thisRef = std::make_unique<Divide>(std::move(Linner), std::move(Rinner));
        return;
    }

    // Convert a / c -> a * (1 / c) to allow further multiply optimizations
    {
        auto L = releaseLeft();
        auto R = releaseRight();
        if (L && R) {
            std::unique_ptr<ASTNode> inv;
            if (R->getType() == ASTNode::Type::Constant) {
                double b = static_cast<Constant*>(R.get())->getValue();
                if (b != 0.0) inv = std::make_unique<Constant>(1.0 / b);
                else inv = std::make_unique<Constant>(0.0);
            } else {
                inv = std::make_unique<Divide>(std::make_unique<Constant>(1.0), std::move(R));
            }
            thisRef = std::make_unique<Multiply>(std::move(L), std::move(inv));
            return;
        }
    }

    // otherwise keep as-is
}

// --- Power ---------------------------------------------------------------
Power::Power(std::unique_ptr<ASTNode> base, std::unique_ptr<ASTNode> exp) : BinaryASTNode(std::move(base), std::move(exp)) {}
ASTNode::Type Power::getType() const { return ASTNode::Type::Power; }
double Power::evaluate(EvaluationContext& ctx) { return std::pow(getLeft().evaluate(ctx), getRight().evaluate(ctx)); }
void Power::accept(ASTVisitor& visitor) { visitor.visit(*this); }

void Power::optimize(std::unique_ptr<ASTNode>& thisRef) {
    if (getMutableLeft()) getMutableLeft()->optimize(getMutableLeft());
    if (getMutableRight()) getMutableRight()->optimize(getMutableRight());

    // both constants
    if (getLeft().getType() == ASTNode::Type::Constant && getRight().getType() == ASTNode::Type::Constant) {
        double a = static_cast<Constant*>(&getLeft())->getValue();
        double b = static_cast<Constant*>(&getRight())->getValue();
        thisRef = std::make_unique<Constant>(std::pow(a,b));
        return;
    }

    // exponent == 0 -> 1
    if (getRight().getType() == ASTNode::Type::Constant) {
        double b = static_cast<Constant*>(&getRight())->getValue();
        if (b == 0.0) { thisRef = std::make_unique<Constant>(1.0); return; }
        if (b == 1.0) { thisRef = releaseLeft(); return; }
        if (b == -1.0) { auto base = releaseLeft(); thisRef = std::make_unique<Divide>(std::make_unique<Constant>(1.0), std::move(base)); return; }
    }

    // base == 0 -> 0
    if (getLeft().getType() == ASTNode::Type::Constant) {
        double a = static_cast<Constant*>(&getLeft())->getValue();
        if (a == 0.0) { thisRef = std::make_unique<Constant>(0.0); return; }
        if (a == 1.0) { thisRef = std::make_unique<Constant>(1.0); return; }
    }

    // otherwise keep as-is (children already optimized)
}
//---------------------------------------------------------------------------
} // namespace ast
