#include "simplevm/simplevm.hpp"
#include <iostream>
#include <vector>
#include <sstream>
#include <limits>

namespace simplevm {

int32_t runVM(const std::vector<std::string>& instructions, std::ostream* out = nullptr)
{
    int32_t A = 0, B = 0, C = 0, D = 0;

    auto print = [&](const std::string& s) {
        if (out) *out << s;
        else std::cerr << s;
    };

    for (const std::string& instruction : instructions) {
        std::istringstream iss(instruction);
        int opcode;
        iss >> opcode;

        if (opcode == 10) { // movi
            char reg;
            int iimm;
            iss >> reg >> iimm;
            if (reg == 'A') A = iimm;
            else if (reg == 'B') B = iimm;
            else if (reg == 'C') C = iimm;
            else if (reg == 'D') D = iimm;
        } else if (opcode == 20) { // mov
            char dest, src;
            iss >> dest >> src;
            int32_t* d = nullptr;
            int32_t* s = nullptr;
            if (dest == 'A') d = &A;
            else if (dest == 'B') d = &B;
            else if (dest == 'C') d = &C;
            else if (dest == 'D') d = &D;
            if (src == 'A') s = &A;
            else if (src == 'B') s = &B;
            else if (src == 'C') s = &C;
            else if (src == 'D') s = &D;
            if (d && s) *d = *s;
        } else if (opcode == 50) { // addi
            A = static_cast<int32_t>(static_cast<int64_t>(A) + B);
        } else if (opcode == 51) { // subi
            A = static_cast<int32_t>(static_cast<int64_t>(A) - B);
        } else if (opcode == 52) { // rsubi
            A = static_cast<int32_t>(static_cast<int64_t>(B) - A);
        } else if (opcode == 53) { // muli
            A = static_cast<int32_t>(static_cast<int64_t>(A) * B);
        } else if (opcode == 54) { // divi
            if (B == 0) {
                print("division by 0\n");
                return A;
            }
            A = A / B;
        } else if (opcode == 63) { // division by 0 signal
            print("division by 0\n");
            return A;
        }
    }

    return A;
}

void fibonacciProgram(unsigned n, std::vector<std::string>& program)
{
    program.clear();
    program.push_back("10 A 0"); // f0
    program.push_back("10 B 1"); // f1
    program.push_back("10 C 0"); // fn

    for (unsigned i = 2; i <= n; i++) {
        program.push_back("20 D A"); // D = A (fn-2)
        program.push_back("20 A B"); // A = B (fn-1)
        program.push_back("50");     // A = A + B
        program.push_back("20 C A"); // C = A (fn)
    }
}

} // namespace simplevm
