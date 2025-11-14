// ...existing code...
#include "simplevm/simplevm.hpp"

#include <array>
#include <cstdint>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

namespace simplevm {

constexpr int OPC_HALT  = 0;
constexpr int OPC_MOVI  = 10;
constexpr int OPC_MOVF  = 11;
constexpr int OPC_MOV   = 20;
constexpr int OPC_STORE = 21;
constexpr int OPC_SWAP  = 22;
constexpr int OPC_ADD3  = 30;
constexpr int OPC_FCPY  = 31;
constexpr int OPC_FSWAP = 32;
constexpr int OPC_ADDI  = 50;
constexpr int OPC_SUBI  = 51;
constexpr int OPC_RSUBI = 52;
constexpr int OPC_MULI  = 53;
constexpr int OPC_DIVI  = 54;
constexpr int OPC_ADDF  = 60;
constexpr int OPC_SUBF  = 61;
constexpr int OPC_MULF  = 62;
constexpr int OPC_DIVF  = 63;
constexpr int OPC_ITOF  = 40;
constexpr int OPC_FTOI  = 41;

inline bool isIntReg(char r) noexcept { return r >= 'A' && r <= 'D'; }
inline bool isFloatReg(char r) noexcept { return r >= 'X' && r <= 'W'; }
inline std::size_t idxInt(char r) noexcept { return static_cast<std::size_t>(r - 'A'); }
inline std::size_t idxFloat(char r) noexcept { return static_cast<std::size_t>(r - 'X'); }

static std::vector<std::string> tokenize(const std::string& line) {
    std::vector<std::string> toks;
    std::istringstream iss(line);
    std::string t;
    while (iss >> t) toks.push_back(std::move(t));
    return toks;
}

// Execute textual program and return register A.
int32_t runVM(const std::vector<std::string>& instructions)
{
    std::array<int32_t,4> I = {0,0,0,0};
    std::array<double,4> F = {0.0,0.0,0.0,0.0};

    for (const auto& line : instructions) {
        if (line.empty()) continue;
        auto tok = tokenize(line);
        if (tok.empty()) continue;
        // first token must be opcode number
        int opcode = 0;
        try { opcode = std::stoi(tok[0]); } catch (...) { continue; }

        switch (opcode) {
        case OPC_HALT:
            return I[0];

        case OPC_MOVI:
            if (tok.size() >= 3 && tok[1].size()==1 && isIntReg(tok[1][0])) {
                I[idxInt(tok[1][0])] = static_cast<int32_t>(std::stol(tok[2]));
            }
            break;

        case OPC_MOVF:
            if (tok.size() >= 3 && tok[1].size()==1 && isFloatReg(tok[1][0])) {
                F[idxFloat(tok[1][0])] = std::stod(tok[2]);
            }
            break;

        // MOV: "20 <Src>" -> load src into A
        // or  "20 <Dest> <Src>" -> dest = src (int->int or float->float)
        case OPC_MOV:
            if (tok.size() == 2 && tok[1].size()==1) {
                char src = tok[1][0];
                if (isIntReg(src)) I[0] = I[idxInt(src)];
                else if (isFloatReg(src)) I[0] = static_cast<int32_t>(F[idxFloat(src)]);
            } else if (tok.size() >= 3 && tok[1].size()==1 && tok[2].size()==1) {
                char dest = tok[1][0], src = tok[2][0];
                if (isIntReg(dest) && isIntReg(src)) I[idxInt(dest)] = I[idxInt(src)];
                else if (isFloatReg(dest) && isFloatReg(src)) F[idxFloat(dest)] = F[idxFloat(src)];
            }
            break;

        case OPC_STORE:
            if (tok.size() >= 2 && tok[1].size()==1) {
                char dest = tok[1][0];
                if (isIntReg(dest)) I[idxInt(dest)] = I[0];
                else if (isFloatReg(dest)) F[idxFloat(dest)] = static_cast<double>(I[0]);
            }
            break;

        case OPC_SWAP:
            std::swap(I[0], I[1]);
            break;

        case OPC_ADD3:
            if (tok.size() >= 4 && tok[1].size()==1 && tok[2].size()==1 && tok[3].size()==1) {
                char d = tok[1][0], r1 = tok[2][0], r2 = tok[3][0];
                if (isIntReg(d) && isIntReg(r1) && isIntReg(r2)) {
                    int64_t t = static_cast<int64_t>(I[idxInt(r1)]) + static_cast<int64_t>(I[idxInt(r2)]);
                    I[idxInt(d)] = static_cast<int32_t>(t);
                }
            }
            break;

        case OPC_FCPY:
            if (tok.size() >= 2 && tok[1].size()==1 && isFloatReg(tok[1][0])) {
                F[idxFloat(tok[1][0])] = F[0];
            }
            break;

        case OPC_FSWAP:
            std::swap(F[0], F[1]);
            break;

        case OPC_ADDI:
            I[0] = static_cast<int32_t>(static_cast<int64_t>(I[0]) + static_cast<int64_t>(I[1]));
            break;
        case OPC_SUBI:
            I[0] = static_cast<int32_t>(static_cast<int64_t>(I[0]) - static_cast<int64_t>(I[1]));
            break;
        case OPC_RSUBI:
            I[0] = static_cast<int32_t>(static_cast<int64_t>(I[1]) - static_cast<int64_t>(I[0]));
            break;
        case OPC_MULI:
            I[0] = static_cast<int32_t>(static_cast<int64_t>(I[0]) * static_cast<int64_t>(I[1]));
            break;
        case OPC_DIVI: {
            int32_t a = I[0], b = I[1];
            if (b == 0) {
                std::cout << "division by 0\n";
            } else {
                I[0] = static_cast<int32_t>(a / b);
                I[1] = static_cast<int32_t>(a % b);
            }
            break;
        }

        case OPC_ADDF:
            F[0] = F[0] + F[1];
            break;
        case OPC_SUBF:
            F[0] = F[0] - F[1];
            break;
        case OPC_MULF:
            F[0] = F[0] * F[1];
            break;
        case OPC_DIVF:
            if (F[1] == 0.0) std::cout << "division by 0\n";
            else F[0] = F[0] / F[1];
            break;

        case OPC_ITOF:
            F[0] = static_cast<double>(I[0]);
            break;
        case OPC_FTOI:
            I[0] = static_cast<int32_t>(F[0]);
            break;

        default:
            break;
        }
    }

    return I[0];
}

// Accept program as newline-separated text.
int32_t runVM(const std::string& programText)
{
    std::vector<std::string> lines;
    std::istringstream iss(programText);
    std::string line;
    while (std::getline(iss, line)) {
        if (!line.empty() && line.back() == '\r') line.pop_back();
        lines.push_back(line);
    }
    return runVM(lines);
}

// Read program from stdin (used by tests).
int32_t runVM()
{
    std::vector<std::string> lines;
    std::string line;
    while (std::getline(std::cin, line)) {
        if (!line.empty() && line.back() == '\r') line.pop_back();
        lines.push_back(line);
    }
    return runVM(lines);
}

// Produce a Fibonacci program as textual instructions. After execution
// register A will contain f(n).
std::vector<std::string> fibonacciProgram(unsigned n)
{
    std::vector<std::string> program;
    program.reserve(2 + n * 3 + 1);

    program.push_back("10 A 0"); // A = f0
    program.push_back("10 B 1"); // B = f1

    for (unsigned i = 0; i < n; ++i) {
        program.push_back("30 C A B"); // C = A + B
        program.push_back("20 A B");   // A = B
        program.push_back("20 B C");   // B = C
    }

    program.push_back("0"); // halt
    return program;
}

} // namespace simplevm
// ...existing code...