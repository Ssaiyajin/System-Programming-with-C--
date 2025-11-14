#ifndef H_lib_AST
#define H_lib_AST
#include "lib/EvaluationContext.hpp"
#include "lib/ASTVisitor.hpp"
#include <cmath>
#include <memory>

namespace ast {

class EvaluationContext;

// Base AST node
class ASTNode {
public:
    enum class Type {
        UnaryPlus, UnaryMinus,
        Add, Subtract, Multiply, Divide, Power,
        Constant, Parameter
    };

    virtual ~ASTNode() = default;
    virtual Type getType() const = 0;
    virtual double evaluate(EvaluationContext& ctx) = 0;
    virtual void optimize(std::unique_ptr<ASTNode>& thisRef) = 0;
    virtual void accept(ASTVisitor& visitor) = 0;
};

// UnaryPlus
class UnaryPlus : public ASTNode {
public:
    UnaryPlus(std::unique_ptr<ASTNode> child);
    Type getType() const override;
    double evaluate(EvaluationContext& ctx) override;
    void optimize(std::unique_ptr<ASTNode>& thisRef) override;
    void accept(ASTVisitor& visitor) override;
    std::unique_ptr<ASTNode>& getMutableInput();
    ASTNode& getInput();
    const ASTNode& getInput() const;
private:
    std::unique_ptr<ASTNode> childNode;
};

// UnaryMinus
class UnaryMinus : public ASTNode {
public:
    UnaryMinus(std::unique_ptr<ASTNode> child);
    Type getType() const override;
    double evaluate(EvaluationContext& ctx) override;
    void optimize(std::unique_ptr<ASTNode>& thisRef) override;
    void accept(ASTVisitor& visitor) override;
    std::unique_ptr<ASTNode>& getMutableInput();
    ASTNode& getInput();
    const ASTNode& getInput() const;
private:
    std::unique_ptr<ASTNode> childNode;
};

// Binary base
class BinaryASTNode : public ASTNode {
public:
    BinaryASTNode(std::unique_ptr<ASTNode> l, std::unique_ptr<ASTNode> r);
    ASTNode& getLeft();
    ASTNode& getRight();
    std::unique_ptr<ASTNode> releaseLeft();
    std::unique_ptr<ASTNode> releaseRight();
    std::unique_ptr<ASTNode>& getMutableLeft();
    std::unique_ptr<ASTNode>& getMutableRight();
protected:
    std::unique_ptr<ASTNode> leftNode;
    std::unique_ptr<ASTNode> rightNode;
};

// Add/Subtract/Multiply/Divide/Power
class Add : public BinaryASTNode {
public:
    Add(std::unique_ptr<ASTNode> l, std::unique_ptr<ASTNode> r);
    Type getType() const override;
    double evaluate(EvaluationContext& ctx) override;
    void optimize(std::unique_ptr<ASTNode>& thisRef) override;
    void accept(ASTVisitor& visitor) override;
};

class Subtract : public BinaryASTNode {
public:
    Subtract(std::unique_ptr<ASTNode> l, std::unique_ptr<ASTNode> r);
    Type getType() const override;
    double evaluate(EvaluationContext& ctx) override;
    void optimize(std::unique_ptr<ASTNode>& thisRef) override;
    void accept(ASTVisitor& visitor) override;
};

class Multiply : public BinaryASTNode {
public:
    Multiply(std::unique_ptr<ASTNode> l, std::unique_ptr<ASTNode> r);
    Type getType() const override;
    double evaluate(EvaluationContext& ctx) override;
    void optimize(std::unique_ptr<ASTNode>& thisRef) override;
    void accept(ASTVisitor& visitor) override;
};

class Divide : public BinaryASTNode {
public:
    Divide(std::unique_ptr<ASTNode> l, std::unique_ptr<ASTNode> r);
    Type getType() const override;
    double evaluate(EvaluationContext& ctx) override;
    void optimize(std::unique_ptr<ASTNode>& thisRef) override;
    void accept(ASTVisitor& visitor) override;
};

class Power : public BinaryASTNode {
public:
    Power(std::unique_ptr<ASTNode> base, std::unique_ptr<ASTNode> exp);
    Type getType() const override;
    double evaluate(EvaluationContext& ctx) override;
    void optimize(std::unique_ptr<ASTNode>& thisRef) override;
    void accept(ASTVisitor& visitor) override;
};

// Constant
class Constant : public ASTNode {
public:
    Constant(double v);
    Type getType() const override;
    double evaluate(EvaluationContext& ctx) override;
    void optimize(std::unique_ptr<ASTNode>& thisRef) override;
    void accept(ASTVisitor& visitor) override;
    double getValue() const;
private:
    double value;
};

// Parameter
class Parameter : public ASTNode {
public:
    Parameter(size_t idx);
    Type getType() const override;
    double evaluate(EvaluationContext& ctx) override;
    void optimize(std::unique_ptr<ASTNode>& thisRef) override;
    void accept(ASTVisitor& visitor) override;
    size_t getIndex() const;
private:
    size_t m_index;
};

} // namespace ast
#endif
