#include "lib/AST.hpp"
#include "lib/EvaluationContext.hpp"
#include "lib/ASTVisitor.hpp"
#include <cmath>
//---------------------------------------------------------------------------
namespace ast {

// AST Node

ASTNode* ASTNode::releaseInput() {
    return childNode.release();
}

ASTNode& ASTNode::getInput() {
    return *childNode;
}


// Constant implementation

Constant::Constant(double value) : value(value) {}

double Constant::getValue() const {
    return value;
}


ASTNode::Type Constant::getType() const {
    return ASTNode::Type::Constant;
}

double Constant::evaluate(EvaluationContext& context) {
    (void) context;
    return getValue();  // Simply return the stored value for Constant
}

void Constant::accept(ASTVisitor& visitor) {
    visitor.visit(*this);
}

void Constant::optimize(std::unique_ptr<ASTNode>&  thisRef) {
    // No specific optimization for Constant in this example
    // Constant values do not change, so no need for optimization
    (void) thisRef;
}

// Parameter implementation
 Parameter::Parameter(size_t index) : m_index(index) {
        // Initialize index to a default value, adjust as needed
    }

ASTNode::Type Parameter::getType() const  {
    return ASTNode::Type::Parameter;
}

double Parameter::evaluate(EvaluationContext& context) {
        // Implement the logic to retrieve the parameter value from the context
        // Use the Parameter's index to fetch the corresponding value
        return context.getParameter(m_index);
    }

size_t Parameter::getIndex() {
    return m_index;
}
void Parameter::accept(ASTVisitor& visitor) {
    visitor.visit(*this);
}

void Parameter::optimize(std::unique_ptr<ASTNode>& thisRef ) {
    // No specific optimization for Parameter in this example
    (void) thisRef;
}

// UnaryPlus implementation
UnaryPlus::UnaryPlus(std::unique_ptr<ASTNode> child) : childNode(std::move(child)) {}

ASTNode::Type UnaryPlus::getType() const {
    return ASTNode::Type::UnaryPlus;
}

double UnaryPlus::evaluate(EvaluationContext& context) {
    return childNode->evaluate(context);
}

void UnaryPlus::optimize(std::unique_ptr<ASTNode>& thisRef) {
    childNode->optimize(childNode);
    
    if (childNode->getType() == Type::UnaryPlus ||
        childNode->getType() == Type::Parameter) {
        thisRef = std::move(childNode); // Transfer ownership to thisRef
    }
}


void UnaryPlus::accept(ASTVisitor& visitor) {
    // if (childNode) {
    //     childNode->accept(visitor);
    // }
    visitor.visit(*this);
}

std::unique_ptr<ASTNode>& UnaryPlus::getMutableInput() {
    return childNode;
}
double UnaryPlus::getResult() const {
    return result;  // Return the stored result
}
// BinaryASTNode implementation
BinaryASTNode::BinaryASTNode(std::unique_ptr<ASTNode> left, std::unique_ptr<ASTNode> right)
    : leftNode(std::move(left)), rightNode(std::move(right)) {}

ASTNode& BinaryASTNode::getLeft() {
    return *leftNode;
}

ASTNode& BinaryASTNode::getRight()  {
    return *rightNode;
}

std::unique_ptr<ASTNode> BinaryASTNode::releaseLeft() {
    return std::move(leftNode);
}

std::unique_ptr<ASTNode> BinaryASTNode::releaseRight() {
    return std::move(rightNode);
}

// Add implementation
Add::Add(std::unique_ptr<ASTNode> left, std::unique_ptr<ASTNode> right)
    : BinaryASTNode(std::move(left), std::move(right)) {}

ASTNode::Type Add::getType() const {
    return ASTNode::Type::Add;
}

double Add::evaluate(EvaluationContext& context)  {
    return getLeft().evaluate(context) + getRight().evaluate(context);
}
void Add::optimize(std::unique_ptr<ASTNode>& thisRef) {

    // Optimize the left and right child nodes
    getMutableLeft()->optimize(left);
    getMutableRight()->optimize(right);
    // Check if left and right are not null
        // Check if both children are constants
        if (getLeft().getType() == ASTNode::Type::Constant && getRight().getType() == ASTNode::Type::Constant) {
            // Apply addition directly to the constant values
            double prev_left = static_cast<Constant*>(&getLeft())->getValue();
            double prev_right = static_cast<Constant*>(&getRight())->getValue();
            thisRef = std::make_unique<Constant>(prev_left + prev_right);
        } else if (getLeft().getType() == ASTNode::Type::Constant) {
            double prev_left = static_cast<Constant*>(&getLeft())->getValue();
            // Handle case where left operand is constant and equals zero
            if (prev_left == 0.0) {
            // Release ownership of the left child and reset thisRef
                thisRef = releaseRight();
            }
        } else if (getRight().getType() == Type::Constant) {
            double prev_right = static_cast<Constant*>(&getRight())->getValue();
            if (prev_right == 0.0) {
                // Release ownership of the right child and reset thisRef
                thisRef = releaseLeft();
            }
        } else if (getLeft().getType() == Type::UnaryMinus) {
            std::unique_ptr<ASTNode> prevLeftChild(static_cast<BinaryASTNode*>(&getLeft())->releaseInput());
            std::unique_ptr<ASTNode> prevRightChild(releaseRight());
            thisRef = std::make_unique<Subtract>(std::move(prevRightChild), std::move(prevLeftChild));

    } else if (getRight().getType() == Type::UnaryMinus) {
        std::unique_ptr<ASTNode> prevLeftChild(releaseLeft());
        std::unique_ptr<ASTNode> prevRightChild(static_cast<BinaryASTNode*>(&getRight())->releaseInput());

        thisRef = std::make_unique<Subtract>(std::move(prevLeftChild), std::move(prevRightChild));
    }
}


void Add::accept(ASTVisitor& visitor) {
    visitor.visit(*this);
    // Implement any additional logic related to visiting an Add node
}
std::unique_ptr<ASTNode>& Add::getMutableLeft() {
    return leftNode;
}

std::unique_ptr<ASTNode>& Add::getMutableRight() {
    return rightNode;
}
// UnaryMinus implementation
UnaryMinus::UnaryMinus(std::unique_ptr<ASTNode> child) : childNode(std::move(child)) {}

ASTNode::Type UnaryMinus::getType() const   {
    return ASTNode::Type::UnaryMinus;
}

double UnaryMinus::evaluate(EvaluationContext& context)   {
    return -childNode->evaluate(context);
}

void UnaryMinus::optimize(std::unique_ptr<ASTNode>& thisRef) {
    // Optimize the child node first
    childNode->optimize(childNode);

    // Check if the child node is a constant
    if (childNode->getType() == ASTNode::Type::Constant) {
        // Apply unary minus to the constant
        auto constantValue = dynamic_cast<Constant*>(childNode.get())->getValue();
        thisRef = std::make_unique<Constant>(-constantValue);
    } else if (childNode->getType() == ASTNode::Type::UnaryMinus) {
        // If the child node is UnaryMinus, flatten it by skipping one level
        auto input = std::move(dynamic_cast<UnaryMinus*>(childNode.get())->getMutableInput());
        thisRef.reset(input.release());
        //thisRef = std::move(input);
    } else if (childNode->getType() == ASTNode::Type::Subtract) {
    auto subtractNode = dynamic_cast<Subtract*>(childNode.get());

    // Check if both operands are constants
    if (subtractNode->getMutableLeft()->getType() == ASTNode::Type::UnaryMinus &&
        subtractNode->getMutableRight()->getType() == ASTNode::Type::Constant) {
        auto leftValue = dynamic_cast<Constant*>(subtractNode->getMutableLeft().get())->getValue();
        auto rightValue = dynamic_cast<Constant*>(subtractNode->getMutableRight().get())->getValue();
        thisRef = std::make_unique<Constant>(rightValue - leftValue);
    } else if (subtractNode->getMutableLeft()->getType() == ASTNode::Type::UnaryMinus) {
        std::unique_ptr<ASTNode> prevLeftChild(static_cast<BinaryASTNode*>(subtractNode->getMutableLeft().get())->releaseInput());
        auto right = std::move(subtractNode->getMutableRight());
        thisRef = std::make_unique<Add>(std::move(right), std::move(prevLeftChild));
    } else {
        // If not both constants or left is UnaryMinus, create a new Subtract node
        auto left = std::move(subtractNode->getMutableLeft());
        auto right = std::move(subtractNode->getMutableRight());
        thisRef = std::make_unique<Subtract>(std::move(left), std::move(right));
    }
} else {
    // If the child node is not a constant, UnaryMinus, or Subtract, create a new UnaryMinus node
    thisRef = std::make_unique<UnaryMinus>(std::move(childNode));
}
}



double UnaryMinus::getResult() const {
    return result;  // Return the stored result
}
void UnaryMinus::accept(ASTVisitor& visitor) {
    visitor.visit(*this);
    // Implement any additional logic related to visiting a UnaryMinus node
}

std::unique_ptr<ASTNode>& UnaryMinus::getMutableInput() {
    return childNode;
}

// void UnaryMinus::accept(ASTVisitor& visitor)   {
//     visitor.visit(*this);
// }

// Subtract implementation
Subtract::Subtract(std::unique_ptr<ASTNode> left, std::unique_ptr<ASTNode> right)
    : BinaryASTNode(std::move(left), std::move(right)) {}

ASTNode::Type Subtract::getType() const {
    return ASTNode::Type::Subtract;
}

double Subtract::evaluate(EvaluationContext& context) {
    return getLeft().evaluate(context) - getRight().evaluate(context);
}

void Subtract::optimize(std::unique_ptr<ASTNode>& thisRef) {
    // Optimize the left and right child nodes
    getMutableLeft()->optimize(left);
    getMutableRight()->optimize(right);

    // Check if both children are constants
    if (getLeft().getType() == ASTNode::Type::Constant && getRight().getType() == ASTNode::Type::Constant) {
        double prev_left = static_cast<Constant*>(&getLeft())->getValue();
        double prev_right = static_cast<Constant*>(&getRight())->getValue();

        // Replace this node with a Constant node containing the result
        thisRef = std::make_unique<Constant>(prev_left - prev_right);
    } else if (getLeft().getType() == ASTNode::Type::Constant) {
        double prev_left = static_cast<Constant*>(&getLeft())->getValue();

        // Handle case where left operand is constant and equals zero
        if (prev_left == 0.0) {
            std::unique_ptr<ASTNode> prevRightChild(releaseRight());
            thisRef = std::make_unique<UnaryMinus>(std::move(prevRightChild));
            //thisRef = releaseRight();
        }
    } else if (getRight().getType() == ASTNode::Type::Constant) {
        double prev_right = static_cast<Constant*>(&getRight())->getValue();

        // Handle case where right operand is constant and equals zero
        if (prev_right == 0.0) {
            // Release ownership of the right child and reset thisRef
            thisRef.reset(releaseLeft().release());
        }
    } else if (getRight().getType() == ASTNode::Type::UnaryMinus) {
        // Handle case where right operand is a UnaryMinus
        std::unique_ptr<ASTNode> prevLeftChild = releaseLeft();
        std::unique_ptr<ASTNode> prevRightChild = std::unique_ptr<ASTNode>(static_cast<UnaryMinus*>(&getRight())->releaseInput());


        // Replace this node with an Add node
        thisRef = std::make_unique<Add>(std::move(prevLeftChild), std::move(prevRightChild));
    }
}

void Subtract::accept(ASTVisitor& visitor) {
    visitor.visit(*this);
    // Implement any additional logic related to visiting a Divide node
}
std::unique_ptr<ASTNode>& Subtract::getMutableLeft() {
    return leftNode;
}

std::unique_ptr<ASTNode>& Subtract::getMutableRight() {
    return rightNode;
}

// Multiply implementation
Multiply::Multiply(std::unique_ptr<ASTNode> left, std::unique_ptr<ASTNode> right)
    : BinaryASTNode(std::move(left), std::move(right)) {}

ASTNode::Type Multiply::getType() const {
    return ASTNode::Type::Multiply;
}

double Multiply::evaluate(EvaluationContext& context) {
    return getLeft().evaluate(context) * getRight().evaluate(context);
}

void Multiply::optimize(std::unique_ptr<ASTNode>& thisRef) {
    // Optimize the left and right child nodes
    getMutableLeft()->optimize(left);
    getMutableRight()->optimize(right);

    if (getLeft().getType() == Type::Constant &&
        getRight().getType() == Type::Constant) {
        double prev_left = static_cast<Constant*>(&getLeft())->getValue();
        double prev_right = static_cast<Constant*>(&getRight())->getValue();

        thisRef = std::make_unique<Constant>(prev_left * prev_right);
    } else if (getLeft().getType() == Type::Constant) {
        double prev_left = static_cast<Constant*>(&getLeft())->getValue();
        if (prev_left == 0.0) {
            thisRef = std::make_unique<Constant>(0);
        } else if (prev_left == 1.0) {
            thisRef.reset(getMutableRight().release());
        }
        // Add more optimization rules as needed
    } else if (getRight().getType() == Type::Constant) {
        double prev_right = static_cast<Constant*>(&getRight())->getValue();
        if (prev_right == 0.0) {
            thisRef = std::make_unique<Constant>(0);
        } else if (prev_right == 1.0) {
            thisRef.reset(getMutableLeft().release());
        }
        // Add more optimization rules as needed
    }  else if (getRight().getType() == Type::UnaryMinus &&
               getLeft().getType() == Type::UnaryMinus) {
        std::unique_ptr<ASTNode> prevLeftChild(static_cast<BinaryASTNode*>(&getLeft())->releaseInput());
        std::unique_ptr<ASTNode> prevRightChild(static_cast<BinaryASTNode*>(&getRight())->releaseInput());
        thisRef = std::make_unique<Multiply>(std::move(prevLeftChild), std::move(prevRightChild));
    }
    // Add more optimization rules as needed
}

void Multiply::accept(ASTVisitor& visitor) {
    visitor.visit(*this);
    // Implement any additional logic related to visiting a Divide node
}
std::unique_ptr<ASTNode>& Multiply::getMutableLeft() {
    return leftNode;
}

std::unique_ptr<ASTNode>& Multiply::getMutableRight() {
    return rightNode;
}

// Divide implementation
Divide::Divide(std::unique_ptr<ASTNode> left, std::unique_ptr<ASTNode> right)
    : BinaryASTNode(std::move(left), std::move(right)) {}

ASTNode::Type Divide::getType() const {
    return ASTNode::Type::Divide;
}

double Divide::evaluate(EvaluationContext& context) {
    double rightValue = getRight().evaluate(context);
    if (rightValue == 0.0) {
        // Handle division by zero error
        // You may throw an exception or handle it according to your needs
    }
    return getLeft().evaluate(context) / rightValue;
}

void Divide::optimize(std::unique_ptr<ASTNode>& thisRef) {
    // Optimize the left and right child nodes
    getMutableLeft()->optimize(left);
    getMutableRight()->optimize(right);

    if (getLeft().getType() == Type::Constant &&
        getRight().getType() == Type::Constant) {
        double prev_left = static_cast<Constant*>(&getLeft())->getValue();
        double prev_right = static_cast<Constant*>(&getRight())->getValue();

        // Check for division by zero
        if (prev_right != 0.0) {
            thisRef = std::make_unique<Constant>(prev_left / prev_right);
        } else {
            // Handle division by zero case (optional, based on your requirements)
            // For now, setting it to zero for simplicity.
            thisRef = std::make_unique<Constant>(0.0);
        }
    } else if (getLeft().getType() == Type::Constant) {
        double prev_left = static_cast<Constant*>(&getLeft())->getValue();
        if (prev_left == 0.0) {
            thisRef = std::make_unique<Constant>(0);
        }
        // You can add more optimization rules based on your requirements
    } else if (getRight().getType() == Type::Constant) {
    double prev_right = static_cast<Constant*>(&getRight())->getValue();
    if (prev_right == 1.0) {
        thisRef = std::move(getMutableLeft());
    } else {
        std::unique_ptr<ASTNode> prevLeftChild = releaseLeft();

        auto new_right = std::make_unique<Constant>(1 / prev_right);
        thisRef = std::make_unique<Multiply>(std::move(prevLeftChild), std::move(new_right));
    }
    } else if (getRight().getType() == Type::UnaryMinus &&  getLeft().getType() == Type::UnaryMinus) {
    std::unique_ptr<ASTNode> prevLeftChild(static_cast<BinaryASTNode*>(&getLeft())->releaseInput());
    std::unique_ptr<ASTNode> prevRightChild(static_cast<BinaryASTNode*>(&getRight())->releaseInput());
    thisRef = std::make_unique<Divide>(std::move(prevLeftChild), std::move(prevRightChild));
}
    // You can add more optimization rules based on your requirements
}

void Divide::accept(ASTVisitor& visitor) {
    visitor.visit(*this);
    // Implement any additional logic related to visiting a Divide node
}
std::unique_ptr<ASTNode>& Divide::getMutableLeft() {
    return leftNode;
}

std::unique_ptr<ASTNode>& Divide::getMutableRight() {
    return rightNode;
}

// Power implementation
Power::Power(std::unique_ptr<ASTNode> base, std::unique_ptr<ASTNode> exponent)
    : BinaryASTNode(std::move(base), std::move(exponent)) {}

ASTNode::Type Power::getType() const {
    return ASTNode::Type::Power;
}

double Power::evaluate(EvaluationContext& context) {
    return std::pow(getLeft().evaluate(context), getRight().evaluate(context));
}

void Power::optimize(std::unique_ptr<ASTNode>& thisRef) {
    // Optimize the left and right child nodes
    getMutableBase()->optimize(left);
    getMutableExponent()->optimize(right);

    if (getLeft().getType() == Type::Constant &&  getRight().getType() == Type::Constant) {
        double prev_left = static_cast<Constant*>(&getLeft())->getValue();
        double prev_right = static_cast<Constant*>(&getRight())->getValue();

        thisRef = std::make_unique<Constant>(std::pow(prev_left, prev_right));
    } else if (getLeft().getType() == Type::Constant) {
        double prev_left = static_cast<Constant*>(&getLeft())->getValue();
        if (prev_left == 0.0) {
            thisRef = std::make_unique<Constant>(0.0);
        } else if (prev_left == 1.0) {
            thisRef = std::make_unique<Constant>(1.0);
        } else if (prev_left == -1.0) {
            // Handle special case for base being -1
            thisRef = std::make_unique<Constant>((int(getRight().getType()) % 2 == 0) ? 1.0 : -1.0);
        }
    } else if (getRight().getType() == Type::Constant) {
        double prev_right = static_cast<Constant*>(&getRight())->getValue();
        if (prev_right == 1.0) {
            thisRef.reset(this->releaseLeft().release());
        } else if (prev_right == 0.0) {
            thisRef = std::make_unique<Constant>(1.0);
        } else if (prev_right == -1.0) {
            // Handle special case for exponent being -1
            std::unique_ptr<ASTNode> prevLeftChild(releaseLeft());
            thisRef = std::make_unique<Divide>(std::make_unique<Constant>(1.0), std::move(prevLeftChild));
        }
    } else if (getRight().getType() == Type::UnaryMinus &&  getLeft().getType() == Type::UnaryMinus) {
        std::unique_ptr<ASTNode> prevLeftChild(static_cast<BinaryASTNode*>(&getLeft())->releaseInput());
        std::unique_ptr<ASTNode> prevRightChild(static_cast<BinaryASTNode*>(&getRight())->releaseInput());
        thisRef = std::make_unique<Divide>(std::move(prevLeftChild), std::move(prevRightChild));
    }
    // Add more optimization rules as needed
}

void Power::accept(ASTVisitor& visitor) {
    visitor.visit(*this);
    // Implement any additional logic related to visiting a Power node
}
std::unique_ptr<ASTNode>& Power::getMutableBase() {
    return leftNode;
}

std::unique_ptr<ASTNode>& Power::getMutableExponent() {
    return rightNode;
}
} // namespace ast
//---------------------------------------------------------------------------
