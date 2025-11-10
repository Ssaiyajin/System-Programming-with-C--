cmake_minimum_required(VERSION 3.13)
project(sort_pointers)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)
include(Infrastructure)

add_subdirectory(lib)
add_subdirectory(test)
