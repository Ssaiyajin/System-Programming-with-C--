#ifndef H_lib_AST
#define H_lib_AST
#include "lib/EvaluationContext.hpp"
#include "lib/ASTVisitor.hpp" // Include ASTVisitor

//---------------------------------------------------------------------------
#include <cmath>
#include <memory>
//---------------------------------------------------------------------------
namespace ast {
//---------------------------------------------------------------------------
// Forward declarations

class EvaluationContext;

// Base class for AST nodes
class ASTNode {
public:
    enum class Type {
        UnaryPlus,
        UnaryMinus,
        Add,
        Subtract,
        Multiply,
        Divide,
        Power,
        Constant,
        Parameter
    };

    virtual ~ASTNode() = default;
    virtual Type getType() const = 0;
    virtual double evaluate(EvaluationContext& context) = 0;
    virtual void optimize(std::unique_ptr<ASTNode>& thisRef) = 0;
    virtual void accept(ASTVisitor& visitor) = 0;
    // no child storage in base
};

// UnaryPlus
class UnaryPlus : public ASTNode {
public:
    UnaryPlus(std::unique_ptr<ASTNode> child);
    double getResult() const;
    Type getType() const override;
    double evaluate(EvaluationContext& context) override;
    void accept(ASTVisitor& visitor) override;
    void optimize(std::unique_ptr<ASTNode>& thisRef) override;
    std::unique_ptr<ASTNode>& getMutableInput();

private:
    std::unique_ptr<ASTNode> childNode;
    double result;
};

// UnaryMinus
class UnaryMinus : public ASTNode {
public:
    UnaryMinus(std::unique_ptr<ASTNode> child);
    double getResult() const;
    Type getType() const override;
    double evaluate(EvaluationContext& context) override;
    void accept(ASTVisitor& visitor) override;
    void optimize(std::unique_ptr<ASTNode>& thisRef) override;
    std::unique_ptr<ASTNode>& getMutableInput();

private:
    std::unique_ptr<ASTNode> childNode;
    double result;
};

// BinaryASTNode
class BinaryASTNode : public ASTNode {
public:
    BinaryASTNode(std::unique_ptr<ASTNode> left, std::unique_ptr<ASTNode> right);

    ASTNode& getLeft();
    ASTNode& getRight();
    std::unique_ptr<ASTNode> releaseLeft();
    std::unique_ptr<ASTNode> releaseRight();

protected:
    std::unique_ptr<ASTNode> leftNode;
    std::unique_ptr<ASTNode> rightNode;
};

// Add
class Add : public BinaryASTNode {
public:
    Add(std::unique_ptr<ASTNode> left, std::unique_ptr<ASTNode> right);
    void optimize(std::unique_ptr<ASTNode>& thisRef) override;
    Type getType() const override;
    double evaluate(EvaluationContext& context) override;
    void accept(ASTVisitor& visitor) override;

    std::unique_ptr<ASTNode>& getMutableLeft();
    std::unique_ptr<ASTNode>& getMutableRight();
};

// Subtract
class Subtract : public BinaryASTNode {
public:
    Subtract(std::unique_ptr<ASTNode> left, std::unique_ptr<ASTNode> right);
    void optimize(std::unique_ptr<ASTNode>& thisRef) override;
    Type getType() const override;
    double evaluate(EvaluationContext& context) override;
    void accept(ASTVisitor& visitor) override;

    std::unique_ptr<ASTNode>& getMutableLeft();
    std::unique_ptr<ASTNode>& getMutableRight();
};

// Multiply
class Multiply : public BinaryASTNode {
public:
    Multiply(std::unique_ptr<ASTNode> left, std::unique_ptr<ASTNode> right);
    void optimize(std::unique_ptr<ASTNode>& thisRef) override;
    Type getType() const override;
    double evaluate(EvaluationContext& context) override;
    void accept(ASTVisitor& visitor) override;

    std::unique_ptr<ASTNode>& getMutableLeft();
    std::unique_ptr<ASTNode>& getMutableRight();
};

// Divide
class Divide : public BinaryASTNode {
public:
    Divide(std::unique_ptr<ASTNode> left, std::unique_ptr<ASTNode> right);
    void optimize(std::unique_ptr<ASTNode>& thisRef) override;
    Type getType() const override;
    double evaluate(EvaluationContext& context) override;
    void accept(ASTVisitor& visitor) override;

    std::unique_ptr<ASTNode>& getMutableLeft();
    std::unique_ptr<ASTNode>& getMutableRight();
};

// Power
class Power : public BinaryASTNode {
public:
    Power(std::unique_ptr<ASTNode> base, std::unique_ptr<ASTNode> exponent);
    void optimize(std::unique_ptr<ASTNode>& thisRef) override;
    Type getType() const override;
    double evaluate(EvaluationContext& context) override;
    void accept(ASTVisitor& visitor) override;

    std::unique_ptr<ASTNode>& getMutableBase();
    std::unique_ptr<ASTNode>& getMutableExponent();
};

// Constant
class Constant : public ASTNode {
public:
    Constant(double value);
    void optimize(std::unique_ptr<ASTNode>& thisRef) override;
    Type getType() const override;
    double evaluate(EvaluationContext& context) override;
    void accept(ASTVisitor& visitor) override;

    double getValue() const;

private:
    double value;
};

// Parameter
class Parameter : public ASTNode {
public:
    Parameter(size_t index);
    size_t getIndex() const;
    void optimize(std::unique_ptr<ASTNode>& thisRef) override;
    Type getType() const override;
    double evaluate(EvaluationContext& context) override;
    void accept(ASTVisitor& visitor) override;
private:
    size_t m_index;
};

//---------------------------------------------------------------------------
} // namespace ast
//--------------------------------------------------------------------------- 
#endif
