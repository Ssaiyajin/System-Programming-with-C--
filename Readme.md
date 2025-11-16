# System Programming with C++

A collection of small, focused C++ projects that explore **systems-level programming**, including data structures, custom allocators, bit-level containers, a tiny virtual machine, and tooling such as static analysis and unit testing. Each directory in this repository is a **self-contained CMake project** with its own build logic, tests, and examples.

---

## 📦 Contents (Selected)

Each subdirectory is a standalone project.

* **arithmetic_ast** — AST + evaluator for a small arithmetic language
* **arithmetic_types** — Complex & Rational number types with tests
* **binary_max_heap** — Binary heap (max-heap) implementation
* **bitset_container** — Custom BitSet container
* **chaining_hash_table** — Hash table with chaining
* **indirect_sorting** — Pointer- and index-based sorting utilities
* **pooled_list_allocator** — Custom memory allocator + benchmarks
* **simple_vm** — Tiny instruction-based virtual machine & assembler helpers
* **float_binary_stack**, **raii_temp_files**, **thread_safe_multimap**, ... — Additional exercises & examples
* **cmake/** — Shared helper modules (GoogleTest, clang-tidy, benchmarks)

---

## 🛠 Prerequisites

* **CMake 3.13+**
* **C++17-compatible compiler** (GCC, Clang, MSVC)
* **Ninja or Make** (optional)
* **Python** (for some benchmarks/tools)

Recommended tools:

* `clang-format`
* `clang-tidy`
* GoogleTest (bundled under `cmake/thirdparty`)

---

## 🚀 Quick Build (All Projects)

### Linux / macOS

```bash
mkdir -p build && cd build
cmake -S .. -B . -DCMAKE_BUILD_TYPE=Debug
cmake --build . -- -j$(nproc)
```

### Windows (PowerShell)

```powershell
mkdir build; cd build
cmake -S .. -B . -A x64 -DCMAKE_BUILD_TYPE=Debug
cmake --build . -- /m
```

---

## 🧪 Running Tests

> ⚠ **Note:** This repository does **not** use a single global CTest configuration. Each subproject produces its own test executables under a `test/` folder (for example `test/tester_*`). CI uses pattern-based discovery to run these.

### Run tests manually

From a subproject build directory:

```bash
ls test
# example output:
# tester_arithmetic
# tester_hash_table

./test/tester_arithmetic
./test/tester_hash_table
```

Common test naming patterns supported by CI:

* `test`
* `./test/tester_*`
* `./test/tester`

If your project builds multiple tester binaries (e.g. `tester_a`, `tester_b`), run them all or use the provided CI pattern logic.

---

## 🏗 Building / Running a Single Subproject

Example: `simple_vm`

```bash
mkdir -p build_simple_vm
cmake -S simple_vm -B build_simple_vm -DCMAKE_BUILD_TYPE=Debug
cmake --build build_simple_vm
```

Run tests (if present):

```bash
./build_simple_vm/test/tester_vm
```

Run example programs:

```bash
./build_simple_vm/simple_vm_example
```

---

## 📊 Benchmarks

Some subprojects include `benchmark/` directories using Google Benchmark (bundled in `cmake/thirdparty`). Build and run targets produced by CMake:

```bash
cmake --build build --target benchmark_pooled_list_allocator
./build/benchmark/benchmark_pooled_list_allocator
```

---

## 🧹 Code Quality Tools

### clang-format

```bash
clang-format -i $(find . -name '*.cpp' -o -name '*.hpp')
```

### clang-tidy

Run via CMake:

```bash
cmake -S . -B build -DCMAKE_CXX_CLANG_TIDY="clang-tidy"
cmake --build build
```

Configuration files included in the repo:

* `.clang-format`
* `.clang-tidy`
* `cmake/FindClangTidy.cmake`

---

## 🔍 Common Issues & Tips

### "No test executable found"

Some projects intentionally produce multiple test binaries (e.g. `tester_a`, `tester_b`) or name them inconsistently. The CI uses patterns to find test executables; if none match the patterns, testing for that project will be skipped and the job will continue.

Example local discovery command:

```bash
find build -type f -executable -name "tester*"
```

### clang-tidy warnings

Several examples intentionally demonstrate code patterns that trigger static analysis warnings for educational purposes. Suppress with `// NOLINT` where appropriate or update `.clang-tidy` to relax specific checks.

---

## 🤖 GitHub Actions (CI) Integration

CI jobs are structured per-project. Each job builds the project into `build_${{ matrix.project }}` and then attempts to run test executables using flexible patterns, for example:

```bash
patterns=( "test" "test/tester_*" "test/tester" )
for pat in "${patterns[@]}"; do
  for exe in $pat; do
    if [ -f "$exe" ] && [ -x "$exe" ]; then
      echo "Running $exe"
      "$exe"
    fi
  done
done
```

This avoids needing a global `ctest` setup and allows each subproject to maintain its independent test layout.

---

### Author ✨

Nihar Sawant – aspiring DevOps & Software Engineer with interest in machine learning, cloud, and automation.