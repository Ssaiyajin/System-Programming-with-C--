#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace simplevm {

// Execute the given program (list of textual instructions). Returns register A.
int32_t runVM(const std::vector<std::string>& instructions);

// Convenience overloads:
// - run a program represented as newline-separated text
// - read a program from std::cin (used by tests)
int32_t runVM(const std::string& programText);
int32_t runVM();

// Produce a fibonacci program as a sequence of textual instructions.
//
// The returned vector contains textual instructions, suitable for runVM().
std::vector<std::string> fibonacciProgram(unsigned n);

} // namespace simplevm
