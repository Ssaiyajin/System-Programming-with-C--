#include "simplevm/simplevm.hpp"
#include <iostream>
#include <vector>
#include <sstream>

namespace simplevm {

int32_t runVM()
{
    int A = 0, B = 0, C = 0, D = 0;

    std::vector<std::string> instructions;

    for (const std::string& instruction : instructions) {
        std::istringstream iss(instruction);
        int opcode;
        iss >> opcode;

        if (opcode == 10) {
            char reg;
            int iimm;
            iss >> reg >> iimm;
            if (reg == 'A') A = iimm;
            else if (reg == 'B') B = iimm;
            else if (reg == 'C') C = iimm;
            else if (reg == 'D') D = iimm;
        } else if (opcode == 20) {
            char dest, src;
            iss >> dest >> src;
            if (dest == 'A') {
                if (src == 'A') A = A;
                else if (src == 'B') A = B;
                else if (src == 'C') A = C;
                else if (src == 'D') A = D;
            } else if (dest == 'B') {
                if (src == 'A') B = A;
                else if (src == 'B') B = B;
                else if (src == 'C') B = C;
                else if (src == 'D') B = D;
            } else if (dest == 'C') {
                if (src == 'A') C = A;
                else if (src == 'B') C = B;
                else if (src == 'C') C = C;
                else if (src == 'D') C = D;
            } else if (dest == 'D') {
                if (src == 'A') D = A;
                else if (src == 'B') D = B;
                else if (src == 'C') D = C;
                else if (src == 'D') D = D;
            }
        } else if (opcode == 50) {
            char reg;
            iss >> reg;
            if (reg == 'A') A += 1;
            else if (reg == 'B') B += 1;
            else if (reg == 'C') C += 1;
            else if (reg == 'D') D += 1;
        } else if (opcode == 63) {
            std::cerr << "division by 0" << std::endl;
            return A;
        }
    }

    return A;
}

void fibonacciProgram(unsigned n)
{
    std::vector<std::string> program;
    program.push_back("10A0"); // Set f0 to 0
    program.push_back("10B1"); // Set f1 to 1
    program.push_back("10C0"); // Set fn to 0

    for (unsigned i = 2; i <= n; i++) {
        program.push_back("20D A"); // D = A (fn-2)
        program.push_back("20A B"); // A = B (fn-1)
        program.push_back("20B C"); // B = C (fn)
        program.push_back("50C");   // C = A + 1 (fn+1)
    }

    for (const std::string& instruction : program) {
        std::cout << instruction << std::endl;
    }
}

} // namespace simplevm
