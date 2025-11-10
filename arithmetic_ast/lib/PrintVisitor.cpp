#include "lib/PrintVisitor.hpp"
#include "lib/AST.hpp"
#include <iostream>

//---------------------------------------------------------------------------
namespace ast {
void PrintVisitor::visit(UnaryPlus& node) {
    std::cout << "(+";
        ASTNode* inputNode = node.getMutableInput().get();
        if (inputNode) {
            inputNode->accept(*this);
        }
        std::cout << ")";
}


void PrintVisitor::visit( UnaryMinus& node) {
    std::cout << "(-";
        ASTNode* inputNode = node.getMutableInput().get();
        if (inputNode) {
            inputNode->accept(*this);
        }
        std::cout << ")";
}

void PrintVisitor::visit( Add& node) {
    std::cout << "(";
    node.getMutableLeft()->accept(*this);  // Recursively visit the left operand
    std::cout << " + ";
    node.getMutableRight()->accept(*this); // Recursively visit the right operand
    std::cout << ")";
}


void PrintVisitor::visit( Subtract& node) {
    std::cout << "(";
    node.getMutableLeft()->accept(*this);  // Recursively visit the left operand
    std::cout << " - ";
    node.getMutableRight()->accept(*this); // Recursively visit the right operand
    std::cout << ")";
}

void PrintVisitor::visit(Multiply& node) {
    std::cout << "(";
    node.getMutableLeft()->accept(*this);  // Recursively visit the left operand
    std::cout << " * ";
    node.getMutableRight()->accept(*this); // Recursively visit the right operand
    std::cout << ")";
}

void PrintVisitor::visit(Divide& node) {
    std::cout << "(";
    node.getMutableLeft()->accept(*this);  // Recursively visit the left operand
    std::cout << " / ";
    node.getMutableRight()->accept(*this); // Recursively visit the right operand
    std::cout << ")";
}

void PrintVisitor::visit( Power& node) {
    std::cout << "(";
    node.getMutableBase()->accept(*this);     // Recursively visit the base
    std::cout << " ^ ";
    node.getMutableExponent()->accept(*this); // Recursively visit the exponent
    std::cout << ")";
}

void PrintVisitor::visit(Constant& node) {
    std::cout << node.getValue();  // Corrected: Added parentheses to call the function
}

void PrintVisitor::visit(Parameter& node) {
    std::cout << "P" << node.getIndex();
}


} // namespace ast
//---------------------------------------------------------------------------
