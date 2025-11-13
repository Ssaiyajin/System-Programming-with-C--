cmake_minimum_required(VERSION 3.13)
project(parallel_ht)

# Add custom CMake modules
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../cmake")
include(Infrastructure)
include(BundledBenchmark)

# Add your libraries and tests
add_subdirectory(lib)
add_subdirectory(test)

# Handle benchmark correctly (out-of-tree)
# Option 1: Specify a binary directory
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/../cmake/thirdparty/benchmark"
                 "${CMAKE_BINARY_DIR}/benchmark_build")
