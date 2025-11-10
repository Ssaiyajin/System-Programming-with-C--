cmake_minimum_required(VERSION 3.13)
project(raii)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)
include(Infrastructure)

add_subdirectory(lib)
add_subdirectory(test)

add_executable(raii main.cpp)
target_link_libraries(raii raii_core)
