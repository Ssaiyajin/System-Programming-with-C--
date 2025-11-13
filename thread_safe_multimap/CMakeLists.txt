cmake_minimum_required(VERSION 3.13)
project(parallel_ht)

# Add custom CMake modules
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../cmake")
include(Infrastructure)
include(BundledBenchmark)

# Enable threads
find_package(Threads REQUIRED)

# Add your libraries and tests
add_subdirectory(lib)
add_subdirectory(test)

# Add benchmark (out-of-tree)
add_subdirectory(
    "${CMAKE_CURRENT_SOURCE_DIR}/../cmake/thirdparty/benchmark"
    "${CMAKE_BINARY_DIR}/benchmark_build"
)

# Make sure benchmark links with pthread
set_target_properties(benchmark PROPERTIES
    LINKER_LANGUAGE CXX
)
target_link_libraries(benchmark PUBLIC Threads::Threads)
