cmake_minimum_required(VERSION 3.13)
project(binary_heap)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)
include(Infrastructure)

add_subdirectory(binary_heap)
add_subdirectory(lib)
add_subdirectory(test)
