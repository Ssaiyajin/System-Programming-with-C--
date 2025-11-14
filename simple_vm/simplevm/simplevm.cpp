#include "simplevm/simplevm.hpp"
#include <iostream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <cmath>

namespace simplevm {

struct VM {
    int32_t A = 0, B = 0, C = 0, D = 0;
    float X = 0.0f, Y = 0.0f;
    std::string output;

    int32_t& intReg(char r) {
        switch(r) {
            case 'A': return A;
            case 'B': return B;
            case 'C': return C;
            case 'D': return D;
            default: throw std::runtime_error("Invalid int register");
        }
    }

    float& floatReg(char r) {
        switch(r) {
            case 'X': return X;
            case 'Y': return Y;
            default: throw std::runtime_error("Invalid float register");
        }
    }

    int32_t runProgram(const std::string& program) {
        std::istringstream stream(program);
        std::string line;
        while (std::getline(stream, line)) {
            std::istringstream iss(line);
            int opcode;
            iss >> opcode;
            if (opcode == 10) { // load int immediate
                char r; int val; iss >> r >> val;
                intReg(r) = val;
            }
            else if (opcode == 11) { // load float immediate
                char r; float val; iss >> r >> val;
                floatReg(r) = val;
            }
            else if (opcode == 20) { // move int reg to int reg
                char dest, src; iss >> dest >> src;
                intReg(dest) = intReg(src);
            }
            else if (opcode == 41) { // addf
                X = X + Y;
            }
            else if (opcode == 50) { // addi
                int64_t sum = int64_t(A) + int64_t(B);
                intReg('C') = int32_t(sum); // overflow handled automatically
            }
            else if (opcode == 51) { // subi
                int64_t diff = int64_t(A) - int64_t(B);
                intReg('C') = int32_t(diff);
            }
            else if (opcode == 52) { // rsubi
                int64_t diff = int64_t(B) - int64_t(A);
                intReg('C') = int32_t(diff);
            }
            else if (opcode == 53) { // muli
                int64_t prod = int64_t(A) * int64_t(B);
                intReg('C') = int32_t(prod);
            }
            else if (opcode == 54) { // divi
                if (B == 0) { output = "division by 0\n"; return A; }
                intReg('C') = A / B;
            }
            else if (opcode == 60) { // addf
                X = X + Y;
            }
            else if (opcode == 61) { // subf
                X = X - Y;
            }
            else if (opcode == 62) { // mulf
                X = X * Y;
            }
            else if (opcode == 63) { // divf
                if (Y == 0.0f) { output = "division by 0\n"; return A; }
                X = X / Y;
            }
        }
        return A;
    }
};

int32_t runVM() {
    VM vm;
    return vm.A;
}

} // namespace simplevm
