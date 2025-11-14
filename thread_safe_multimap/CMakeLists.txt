cmake_minimum_required(VERSION 3.13)
project(parallel_ht)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)
include(Infrastructure)
include(BundledBenchmark)

add_subdirectory(lib)
add_subdirectory(test)
add_subdirectory(benchmark)
