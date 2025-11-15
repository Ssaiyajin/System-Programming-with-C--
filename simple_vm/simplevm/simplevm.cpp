#include "simplevm/simplevm.hpp"

#include <array>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

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

// helper predicates to avoid incorrect char-range tests
static auto isIntReg = [](char r) -> bool { return (r >= 'A' && r <= 'D'); };
// valid float regs are W, X, Y, Z (ASCII order W..Z)
static auto isFloatReg = [](char r) -> bool { return (r >= 'W' && r <= 'Z'); };

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

        // register-to-register move or single-arg load into A:
        // 20 <Dest> <Src>   (two-arg: Dest = Src)
        // 20 <Src>          (one-arg: A = Src)
        case 20: {
            char a, b;
            if (iss >> a) {
                if (iss >> b) {
                    // two-arg move: a = dest, b = src
                    char dest = a, src = b;
                    if (isIntReg(dest) && isIntReg(src)) {
                        I[idx_int(dest)] = I[idx_int(src)];
                    } else if (isFloatReg(dest) && isFloatReg(src)) {
                        F[idx_float(dest)] = F[idx_float(src)];
                    }
                } else {
                    // single-arg: load into A from register a
                    char src = a;
                    if (isIntReg(src)) {
                        I[0] = I[idx_int(src)];
                    } else if (isFloatReg(src)) {
                        // load float src into A by truncation toward zero
                        I[0] = static_cast<int32_t>(F[idx_float(src)]);
                    }
                }
            }
            break;
        }

        // single-arg store A -> <Reg>
        // 21 <Dest>
        case 21: {
            char dest;
            if (!(iss >> dest)) break;
            if (isIntReg(dest)) {
                I[idx_int(dest)] = I[0];
            } else if (isFloatReg(dest)) {
                F[idx_float(dest)] = static_cast<double>(I[0]);
            }
            break;
        }

        // swap A and B
        // 22
        case 22: {
            std::swap(I[0], I[1]);
            break;
        }

        // three-arg integer add used by fibonacci sample: 30 <Dest> <R1> <R2>
        case 30: {
            char dest, r1, r2;
            if (iss >> dest >> r1 >> r2) {
                if (isIntReg(dest) && isIntReg(r1) && isIntReg(r2)) {
                    int64_t tmp = static_cast<int64_t>(I[idx_int(r1)]) + static_cast<int64_t>(I[idx_int(r2)]);
                    I[idx_int(dest)] = static_cast<int32_t>(tmp);
                } else if (isFloatReg(dest) && isFloatReg(r1) && isFloatReg(r2)) {
                    double tmp = F[idx_float(r1)] + F[idx_float(r2)];
                    F[idx_float(dest)] = tmp;
                }
            }
            break;
        }

        // float-register helpers:
        // 31 <DestFReg>  : copy X -> DestFReg
        case 31: {
            char dest;
            if (!(iss >> dest)) break;
            if (dest >= 'X' && dest <= 'W') {
                F[idx_float(dest)] = F[0];
            }
            break;
        }

        // swap X and Y
        // 32
        case 32: {
            std::swap(F[0], F[1]);
            break;
        }

        // integer arithmetic on A and B (store result in A)
        // 50 addi: A = A + B
        case 50: {
            int64_t tmp = static_cast<int64_t>(I[0]) + static_cast<int64_t>(I[1]);
            I[0] = static_cast<int32_t>(tmp);
            break;
        }
        // 51 subi: A = A - B
        case 51: {
            int64_t tmp = static_cast<int64_t>(I[0]) - static_cast<int64_t>(I[1]);
            I[0] = static_cast<int32_t>(tmp);
            break;
        }
        // 52 rsubi: A = B - A
        case 52: {
            int64_t tmp = static_cast<int64_t>(I[1]) - static_cast<int64_t>(I[0]);
            I[0] = static_cast<int32_t>(tmp);
            break;
        }
        // 53 muli: A = A * B
        case 53: {
            int64_t tmp = static_cast<int64_t>(I[0]) * static_cast<int64_t>(I[1]);
            I[0] = static_cast<int32_t>(tmp);
            break;
        }
        // 54 divi: A = A / B  (detect div by zero)
        case 54: {
            {
                int32_t a = I[0];
                int32_t b = I[1];
                if (b == 0) {
                    // tests capture stdout
                    std::cout << "division by 0\n";
                } else {
                    // compute quotient and remainder from originals
                    int32_t q = static_cast<int32_t>(a / b);
                    int32_t r = static_cast<int32_t>(a % b);
                    I[0] = q;
                    I[1] = r;
                }
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
                // tests capture stdout
                std::cout << "division by 0\n";
            } else {
                F[0] = F[0] / F[1];
            }
            break;
        }

        // 40 itof: convert A (int) -> X (float)
        case 40: {
            F[0] = static_cast<double>(I[0]);
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
        // trim trailing CR
        if (!line.empty() && (line.back() == '\r')) line.pop_back();
        lines.push_back(line);
    }
    return runVM(lines);
}

// Default-run: read program from std::cin (used by tests)
int32_t runVM()
{
    std::vector<std::string> lines;
    std::string line;
    while (std::getline(std::cin, line)) {
        if (!line.empty() && (line.back() == '\r')) line.pop_back();
        lines.push_back(line);
    }
    return runVM(lines);
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

    // perform the update n times: C = A + B ; A = B ; B = C
    for (unsigned i = 0; i < n; ++i) {
        program.push_back("30 C A B"); // C = A + B
        program.push_back("20 A B");   // A = B
        program.push_back("20 B C");   // B = C
    }

    program.push_back("0");

    // Also emit program text to stdout (tests capture std::cout)
    for (const auto &line : program) {
        std::cout << line << '\n';
    }
    return program;
}

} // namespace simplevm

