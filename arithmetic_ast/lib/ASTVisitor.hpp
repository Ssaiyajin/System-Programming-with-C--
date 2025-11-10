#ifndef H_lib_ASTVisitor
#define H_lib_ASTVisitor
//---------------------------------------------------------------------------
namespace ast {
// Forward declaration of ASTNode
    class ASTNode;
    class UnaryMinus; // Forward declaration
    class UnaryPlus;
    class BinaryASTNode;
    class Constant;
    class Parameter;
    class Add; // Forward declare AddNode
    class Subtract; // Forward declare SubtractNode
    class Multiply; // Forward declare MultiplyNode
    class Divide; // Forward declare DivideNode
    class Power; // Forward declare PowerNode

    class ASTVisitor {
    public:
        virtual void visit(UnaryPlus& node) = 0;
        virtual void visit(UnaryMinus& node) = 0;
        virtual void visit( Add& node) = 0; // Forward declare AddNode
        virtual void visit(Subtract& node) = 0; // Forward declare SubtractNode
        virtual void visit(Multiply& node) = 0; // Forward declare MultiplyNode
        virtual void visit(Divide& node) = 0; // Forward declare DivideNode
        virtual void visit(Power& node) = 0; // Forward declare PowerNode
        virtual void visit(Constant& node) = 0;
        virtual void visit(Parameter& node) = 0;
        ~ASTVisitor() = default;
    };

} // namespace ast
//---------------------------------------------------------------------------
#endif
