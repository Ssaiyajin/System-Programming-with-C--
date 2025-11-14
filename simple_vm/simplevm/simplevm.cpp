#include "simplevm/simplevm.hpp"
#include <iostream>
#include <vector>
#include <sstream>
#include <string>
#include <array>
#include <cstdint>

namespace simplevm {

// helper indexers
static auto idx_int = [](char r) -> std::size_t {
    switch (r) {
        case 'A': return 0;
        case 'B': return 1;
        case 'C': return 2;
        case 'D': return 3;
        default:  return 0;
    }
};
static auto idx_float = [](char r) -> std::size_t {
    switch (r) {
        case 'X': return 0;
        case 'Y': return 1;
        case 'Z': return 2;
        case 'W': return 3;
        default:  return 0;
    }
};

// Run a program given as text instructions. Returns register A.
int32_t runVM(const std::vector<std::string>& instructions)
{
    std::array<int32_t,4> I = {0,0,0,0};
    std::array<double,4> F = {0.0,0.0,0.0,0.0};

    for (const std::string& instruction : instructions) {
        if (instruction.empty()) continue;

        std::istringstream iss(instruction);
        int opcode;
        if (!(iss >> opcode)) continue;

        switch (opcode) {
        case 0: // halt / return A
            return I[0];

        // integer immediate move: 10 <Reg> <Imm>
        case 10: {
            char reg; int32_t imm;
            if (iss >> reg >> imm) I[idx_int(reg)] = imm;
            break;
        }

        // float immediate move: 11 <FReg> <Float>
        case 11: {
            char reg; double imm;
            if (iss >> reg >> imm) F[idx_float(reg)] = imm;
            break;
        }

        // register-to-register move: 20 <Dest> <Src>
        case 20: {
            char dest, src;
            if (!(iss >> dest >> src)) break;
            // integer regs
            if ((dest >= 'A' && dest <= 'D') && (src >= 'A' && src <= 'D')) {
                I[idx_int(dest)] = I[idx_int(src)];
            } else if ((dest >= 'X' && dest <= 'Z') && (src >= 'X' && src <= 'Z')) {
                F[idx_float(dest)] = F[idx_float(src)];
            }
            break;
        }

        // three-arg integer add used by fibonacci sample: 30 <Dest> <R1> <R2>
        case 30: {
            char dest, r1, r2;
            if (iss >> dest >> r1 >> r2) {
                if ((dest >= 'A' && dest <= 'D') && (r1 >= 'A' && r1 <= 'D') && (r2 >= 'A' && r2 <= 'D')) {
                    int64_t tmp = (int64_t)I[idx_int(r1)] + (int64_t)I[idx_int(r2)];
                    I[idx_int(dest)] = static_cast<int32_t>(tmp);
                }
            }
            break;
        }

        // integer arithmetic on A and B (store result in A)
        // 50 addi: A = A + B
        case 50: {
            int64_t tmp = (int64_t)I[0] + (int64_t)I[1];
            I[0] = static_cast<int32_t>(tmp);
            break;
        }
        // 51 subi: A = A - B
        case 51: {
            int64_t tmp = (int64_t)I[0] - (int64_t)I[1];
            I[0] = static_cast<int32_t>(tmp);
            break;
        }
        // 52 rsubi: A = B - A
        case 52: {
            int64_t tmp = (int64_t)I[1] - (int64_t)I[0];
            I[0] = static_cast<int32_t>(tmp);
            break;
        }
        // 53 muli: A = A * B
        case 53: {
            int64_t tmp = (int64_t)I[0] * (int64_t)I[1];
            I[0] = static_cast<int32_t>(tmp);
            break;
        }
        // 54 divi: A = A / B  (detect div by zero)
        case 54: {
            if (I[1] == 0) {
                std::cerr << "division by 0\n";
            } else {
                // C++ integer division truncates toward zero, use that behaviour
                I[0] = static_cast<int32_t>(I[0] / I[1]);
            }
            break;
        }

        // float arithmetic operating on X and Y, result in X
        // 60 addf: X = X + Y
        case 60: {
            F[0] = F[0] + F[1];
            break;
        }
        // 61 subf: X = X - Y
        case 61: {
            F[0] = F[0] - F[1];
            break;
        }
        // 62 mulf: X = X * Y
        case 62: {
            F[0] = F[0] * F[1];
            break;
        }
        // 63 divf: X = X / Y (detect div by zero)
        case 63: {
            if (F[1] == 0.0) {
                std::cerr << "division by 0\n";
            } else {
                F[0] = F[0] / F[1];
            }
            break;
        }

        // 41 ftoi: convert X (float) -> A (int) by truncation toward zero
        case 41: {
            I[0] = static_cast<int32_t>(F[0]);
            break;
        }

        default:
            // unknown opcode: ignore
            break;
        }
    }

    // If program falls through without explicit halt, return A
    return I[0];
}

// Convenience overload: accept program as single string with newlines.
int32_t runVM(const std::string& programText)
{
    std::vector<std::string> lines;
    std::istringstream iss(programText);
    std::string line;
    while (std::getline(iss, line)) {
        // trim trailing/leading spaces simple
        if (!line.empty() && (line.back() == '\r')) line.pop_back();
        lines.push_back(line);
    }
    return runVM(lines);
}

// Convenience default-run that demonstrates a simple program (fibonacci).
int32_t runVM()
{
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

    if (n == 0) {
        program.push_back("0");
        return program;
    }
    if (n == 1) {
        program.push_back("0");
        return program;
    }

    // for i = 2..n: C = A + B ; A = B ; B = C
    for (unsigned i = 2; i <= n; ++i) {
        program.push_back("30 C A B"); // C = A + B
        program.push_back("20 A B");   // A = B
        program.push_back("20 B C");   // B = C
    }
    program.push_back("0");
    return program;
}

} // namespace simplevm
