#include "lib/EvaluationContext.hpp"
//---------------------------------------------------------------------------
namespace ast {

    void EvaluationContext::pushParameter(double value) {
    parameters.push_back(value);
}

double EvaluationContext::getParameter(size_t index) const {
        if (index >= parameters.size()) {
            throw std::out_of_range("Index out of bounds in EvaluationContext");
        }
        return parameters[index];
    }
} // namespace ast
//---------------------------------------------------------------------------
