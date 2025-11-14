#include "simplevm/simplevm.hpp"
#include <iostream>
#include <vector>
#include <sstream>
#include <string>
#include <array>

namespace simplevm {

// Run a program given as text instructions. Returns register A.
int32_t runVM(const std::vector<std::string>& instructions)
{
    auto idx = [](char r) -> std::size_t {
        switch (r) {
            case 'A': return 0;
            case 'B': return 1;
            case 'C': return 2;
            case 'D': return 3;
            default:  return 0;
        }
    };

    std::array<int32_t,4> R = {0,0,0,0};

    for (const std::string& instruction : instructions) {
        if (instruction.empty()) continue;

        std::istringstream iss(instruction);
        int opcode;
        if (!(iss >> opcode)) continue;

        switch (opcode) {
        case 10: { // mov immediate: "10 A 5"
            char reg; int imm;
            if (iss >> reg >> imm) R[idx(reg)] = imm;
            break;
        }
        case 20: { // mov reg->reg: "20 A B" means A = B
            char dest, src;
            if (iss >> dest >> src) R[idx(dest)] = R[idx(src)];
            break;
        }
        case 30: { // add two regs: "30 C A B" => C = A + B
            char dest, r1, r2;
            if (iss >> dest >> r1 >> r2) R[idx(dest)] = R[idx(r1)] + R[idx(r2)];
            break;
        }
        case 50: { // inc register: "50 A" => A++
            char reg;
            if (iss >> reg) ++R[idx(reg)];
            break;
        }
        case 63: { // error / exit
            std::cerr << "division by 0" << std::endl;
            return R[0];
        }
        default:
            // unknown opcode - ignore
            break;
        }
    }

    return R[0];
}

// Convenience default-run that demonstrates a simple program (fibonacci).
int32_t runVM()
{
    // simple demo: compute fib(10) and return A (will be 0) — prefer to return B for fib(n)
    auto program = fibonacciProgram(10);
    return runVM(program);
}

// Produce a Fibonacci program as a sequence of text instructions.
// Program uses registers:
//   A = f(n-2), B = f(n-1), C = f(n)
std::vector<std::string> fibonacciProgram(unsigned n)
{
    std::vector<std::string> program;

    // initialize f0 and f1
    program.push_back("10 A 0"); // A = 0
    program.push_back("10 B 1"); // B = 1

    if (n == 0) return program;
    if (n == 1) return program;

    // for i = 2..n: C = A + B ; A = B ; B = C
    for (unsigned i = 2; i <= n; ++i) {
        program.push_back("30 C A B"); // C = A + B
        program.push_back("20 A B");   // A = B
        program.push_back("20 B C");   // B = C
    }

    return program;
}

} // namespace simplevm
