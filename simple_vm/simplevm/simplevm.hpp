#pragma once
#include <cstdint>
#include <string>
#include <vector>

namespace simplevm {

// Execute the given program (list of textual instructions). Returns register A.
int32_t runVM(const std::vector<std::string>& instructions);

// Convenience runner that builds a small demo program and executes it.
int32_t runVM();

// Produce a fibonacci program as a sequence of textual instructions.
std::vector<std::string> fibonacciProgram(unsigned n);

} // namespace simplevm
