#include "simplevm/simplevm.hpp"
#include <iostream>
#include <vector>
#include <sstream>
#include <stdexcept>

namespace simplevm {

int32_t runVM(const std::vector<std::string>& instructions)
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
        else if (opcode == 20) { // move register
            char dest, src;
            iss >> dest >> src;
            getRegister(dest) = getRegister(src);
        } 
        else if (opcode == 50) { // add: C = A + B
            C = A + B;
        }
        else if (opcode == 51) { // sub: C = A - B
            C = A - B;
        }
        else if (opcode == 52) { // rsub: C = B - A
            C = B - A;
        }
        else if (opcode == 53) { // mul: C = A * B
            C = A * B;
        }
        else if (opcode == 54) { // div: C = A / B
            if (B == 0) {
                std::cerr << "division by 0" << std::endl;
                return A;
            }
            C = A / B;
        }
        else if (opcode == 63) { // exit on error
            std::cerr << "division by 0" << std::endl;
            return A;
        }
    }

    return A;
}

std::vector<std::string> fibonacciProgram(unsigned n)
{
    std::vector<std::string> program;
    program.push_back("10 A 0"); // f0
    program.push_back("10 B 1"); // f1
    program.push_back("10 C 0"); // fn

    for (unsigned i = 2; i <= n; i++) {
        program.push_back("20 D A"); // D = A (fn-2)
        program.push_back("20 A B"); // A = B (fn-1)
        program.push_back("20 B C"); // B = C (fn)
        program.push_back("50 C");   // C = A + B (approx using 50 as placeholder)
    }

    return program;
}

} // namespace simplevm
