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

// Forward declarations
// class ASTNode;
// class UnaryPlus;
// class UnaryMinus;
// class BinaryASTNode;
// class Add;
// class Subtract;
// class Multiply;
// class Divide;
// class Power;
// class Constant;
// class  Parameter;
// class ASTVisitor {
// public:
//     virtual void visit(const UnaryPlus& node) = 0;
//     virtual void visit(const UnaryMinus& node) = 0;
//     virtual void visit(const Add& node) = 0;
//     virtual void visit(const Subtract& node) = 0;
//     virtual void visit(const Multiply& node) = 0;
//     virtual void visit(const Divide& node) = 0;
//     virtual void visit(const Power& node) = 0;
//     virtual void visit(const Constant& node) = 0;
// };
/// Base class for AST nodes
class ASTNode {
    public:
    /// All possible types of ASTNodes
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
    ASTNode* releaseInput();
    ASTNode& getInput();
    virtual ~ASTNode() = default;
    virtual Type getType() const = 0;
    virtual double evaluate(EvaluationContext& context)   = 0;
    virtual void optimize(std::unique_ptr<ASTNode>& thisRef) = 0;
    virtual void accept(ASTVisitor& visitor)   = 0;
    private:
    std::unique_ptr<ASTNode> childNode;
};

// UnaryPlus
class UnaryPlus : public ASTNode {
public:
    UnaryPlus(std::unique_ptr<ASTNode> child);
    double getResult() const; // Add this member function to get the result
    Type getType() const   override;
    double evaluate(EvaluationContext& context)   override;
    void accept(ASTVisitor& visitor)   override;
    void optimize(std::unique_ptr<ASTNode>& thisRef) override;
    std::unique_ptr<ASTNode>& getMutableInput();

private:
    std::unique_ptr<ASTNode> childNode;
    double result; // Add this member variable to store the result
};

// UnaryMinus
class UnaryMinus : public ASTNode {
public:
    UnaryMinus(std::unique_ptr<ASTNode> child);
    double getResult() const; // Add this member function to get the result
    Type getType() const   override;
    double evaluate(EvaluationContext& context)   override;
    void accept(ASTVisitor& visitor)   override;
    void optimize(std::unique_ptr<ASTNode>& thisRef) override;
    std::unique_ptr<ASTNode>& getMutableInput();

private:
    std::unique_ptr<ASTNode> childNode;
    double result; // Add this member variable to store the result
};

// BinaryASTNode
class BinaryASTNode : public ASTNode {
public:
    BinaryASTNode(std::unique_ptr<ASTNode> left, std::unique_ptr<ASTNode> right);
   
      ASTNode& getLeft()  ;
      ASTNode& getRight()  ;
    std::unique_ptr<ASTNode> releaseLeft();
    std::unique_ptr<ASTNode> releaseRight();
     friend class Add; // Allow Add to access private members
     friend class Subtract;
     friend class Multiply;
     friend class Divide;
     friend class Power;
private:
    std::unique_ptr<ASTNode> leftNode;
    std::unique_ptr<ASTNode> rightNode;
};

// Add
class Add : public BinaryASTNode {
public:
    Add(std::unique_ptr<ASTNode> left, std::unique_ptr<ASTNode> right);
    void optimize(std::unique_ptr<ASTNode>& thisRef) override;
    Type getType() const   override;
    double evaluate(EvaluationContext& context)   override;
    void accept(ASTVisitor& visitor)   override;
    
    std::unique_ptr<ASTNode>& getMutableLeft();
    std::unique_ptr<ASTNode>& getMutableRight();
private:
    std::unique_ptr<ASTNode> left;
    std::unique_ptr<ASTNode> right;
};

// Subtract
class Subtract : public BinaryASTNode {
public:
    Subtract(std::unique_ptr<ASTNode> left, std::unique_ptr<ASTNode> right);
    void optimize(std::unique_ptr<ASTNode>& thisRef) override;
    Type getType() const   override;
    double evaluate(EvaluationContext& context)   override;
    void accept(ASTVisitor& visitor)   override;

    std::unique_ptr<ASTNode>& getMutableLeft();
    std::unique_ptr<ASTNode>& getMutableRight();
private:
    std::unique_ptr<ASTNode> left;
    std::unique_ptr<ASTNode> right;
};

// Multiply
class Multiply : public BinaryASTNode {
public:
    Multiply(std::unique_ptr<ASTNode> left, std::unique_ptr<ASTNode> right);
    void optimize(std::unique_ptr<ASTNode>& thisRef) override;
    Type getType() const   override;
    double evaluate(EvaluationContext& context)   override;
    void accept(ASTVisitor& visitor)   override;

    std::unique_ptr<ASTNode>& getMutableLeft();
    std::unique_ptr<ASTNode>& getMutableRight();
private:
    std::unique_ptr<ASTNode> left;
    std::unique_ptr<ASTNode> right;
};

// Divide
class Divide : public BinaryASTNode {
public:
    Divide(std::unique_ptr<ASTNode> left, std::unique_ptr<ASTNode> right);
    void optimize(std::unique_ptr<ASTNode>& thisRef) override;
    Type getType() const   override;
    double evaluate(EvaluationContext& context)   override;
    void accept(ASTVisitor& visitor)   override;

    std::unique_ptr<ASTNode>& getMutableLeft();
    std::unique_ptr<ASTNode>& getMutableRight();
private:
    std::unique_ptr<ASTNode> left;
    std::unique_ptr<ASTNode> right;
};

// Power
class Power : public BinaryASTNode {
public:
    Power(std::unique_ptr<ASTNode> base, std::unique_ptr<ASTNode> exponent);
    void optimize(std::unique_ptr<ASTNode>& thisRef) override;
    Type getType() const   override;
    double evaluate(EvaluationContext& context)   override;
    void accept(ASTVisitor& visitor)   override;

    std::unique_ptr<ASTNode>& getMutableBase();
    std::unique_ptr<ASTNode>& getMutableExponent();
private:
    std::unique_ptr<ASTNode> left;
    std::unique_ptr<ASTNode> right;
};

// Constant
class Constant : public ASTNode {
public:
    Constant(double value);
    void optimize(std::unique_ptr<ASTNode>& thisRef) override;
    Type getType() const   override;
    double evaluate(EvaluationContext& context)   override;
    void accept(ASTVisitor& visitor)   override;

    double getValue() const  ;

private:
    double value;
};

//  Parameter
class  Parameter : public ASTNode {
public:
    size_t getIndex();
    Parameter(size_t index);
    void optimize(std::unique_ptr<ASTNode>& thisRef) override;
    Type getType() const   override;
    double evaluate(EvaluationContext& context)   override;
    void accept(ASTVisitor& visitor)   override;
private:
    size_t m_index; // Declare index as a private member variable
};

//---------------------------------------------------------------------------
} // namespace ast
//---------------------------------------------------------------------------
#endif
