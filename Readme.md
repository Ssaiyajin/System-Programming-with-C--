# System Programming with C++

Collection of small C++ projects and exercises focused on systems-level programming techniques (data structures, allocators, a tiny VM, unit tests, CI and static analysis). Each subdirectory is a self-contained CMake project with tests and examples.

## Contents (selected)
- arithmetic_ast — AST + evaluator for small arithmetic language
- arithmetic_types — Complex / Rational types and tests
- binary_max_heap — binary heap implementation & tests
- bitset_container — custom BitSet implementation & tests
- chaining_hash_table — hash table with chaining
- indirect_sorting — pointer-based sorting utilities & tests
- pooled_list_allocator — custom allocator + benchmarks/tests
- simple_vm — a tiny instruction VM and assembler helpers
- float_binary_stack, raii_temp_files, thread_safe_multimap, ... — additional exercises and examples
- cmake/ — shared CMake helper modules (clang-tidy, googletest, benchmarks)

## Prerequisites
- CMake 3.13+
- A C++17-capable compiler (gcc/clang/MSVC)
- Ninja or Make (optional)
- Python (for some tooling/benchmarks)
- Recommended developer tools:
  - clang-format / clang-tidy (configs included)
  - GoogleTest (bundled via cmake/thirdparty)

## Quick build (all projects)
Run an out-of-source build from the repo root.

Linux / macOS (bash):
````bash
mkdir -p build && cd build
cmake -S .. -B . -DCMAKE_BUILD_TYPE=Debug
cmake --build . -- -j$(nproc)
# Run all CTest tests if configured
ctest --output-on-failure || true
Creating build instructions
Windows (PowerShell):
mkdir build; cd build
cmake -S .. -B . -A x64 -DCMAKE_BUILD_TYPE=Debug
cmake --build . -- /m
ctest --output-on-failure



Notes:

Some subprojects define their own top-level CMake files; the top-level CMake infrastructure will scan and configure subprojects using cmake/Infrastructure.cmake.
If your CI logs show multiple tester_* binaries (e.g. test/tester_a), run all test binaries under build/test or use ctest which runs registered tests.
Build / Run a single subproject
Example: build only simple_vm:

Or run an executable directly:

Static analysis and formatting
clang-tidy configuration: .clang-tidy and helper CMake module cmake/FindClangTidy.cmake are provided.
clang-format config: .clang-format
Run formatting:
Run clang-tidy (example):

Testing
Tests use GoogleTest (bundled via cmake/thirdparty/googletest).
After building, run:
If tests are emitted as multiple tester* executables, run them directly:

Benchmarks
Some projects include benchmark/ directories using Google Benchmark (bundled in cmake/thirdparty). Build and run the benchmark targets produced by CMake.

Common issues & tips
"No such file or directory" when CI tries ./test/tester: your build may produce multiple tester_* binaries (e.g. tester_a). Use find or ctest to locate/run them (see above).
clang-tidy warnings: the repo intentionally exposes many examples that trigger static analysis. Use the included NOLINT annotations where appropriate, or adjust the checks in .clang-tidy.
If clang-tidy emits "logical expression is always false" for char-range checks, prefer explicit predicates (e.g. isIntReg() / isFloatReg()) or correct the character ranges.
Contributing
Create topic branches per change.
Follow the repository clang-format rules before push.
Add/extend tests for bug fixes.