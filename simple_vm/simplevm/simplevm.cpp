#include "simplevm/simplevm.hpp"
#include <iostream>
#include <vector>
#include <sstream>

namespace simplevm {

int32_t runVM()
{
    int A = 0, B = 0, C = 0, D = 0;

    auto getRegister = [&](char r) -> int& {
        switch (r) {
            case 'A': return A;
            case 'B': return B;
            case 'C': return C;
            case 'D': return D;
            default: throw std::runtime_error("Invalid register");
        }
    };

    std::vector<std::string> instructions; // your instructions here

    for (const std::string& instruction : instructions) {
        std::istringstream iss(instruction);
        int opcode;
        iss >> opcode;

        if (opcode == 10) { // load immediate
            char reg;
            int value;
            iss >> reg >> value;
            getRegister(reg) = value;
        } 
        else if (opcode == 20) { // move register to register
            char dest, src;
            iss >> dest >> src;
            getRegister(dest) = getRegister(src);
        } 
        else if (opcode == 50) { // increment
            char reg;
            iss >> reg;
            getRegister(reg) += 1;
        } 
        else if (opcode == 63) { // exit on error
            std::cerr << "division by 0" << std::endl;
            return A;
        }
    }

    return A;
}

void fibonacciProgram(unsigned n)
{
    std::vector<std::string> program;
    program.push_back("10 A 0"); // Set A = 0 (f0)
    program.push_back("10 B 1"); // Set B = 1 (f1)
    program.push_back("10 C 0"); // Set C = 0 (fn)

    for (unsigned i = 2; i <= n; i++) {
        program.push_back("20 D A"); // D = A (fn-2)
        program.push_back("20 A B"); // A = B (fn-1)
        program.push_back("20 B C"); // B = C (fn)
        program.push_back("50 C");   // C = C + 1 (fn+1)
    }

    for (const auto& instr : program) {
        std::cout << instr << std::endl;
    }
}

} // namespace simplevm
