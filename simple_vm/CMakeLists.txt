cmake_minimum_required(VERSION 3.13)
project(simplevm)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)
include(Infrastructure)

add_subdirectory(simplevm)
add_subdirectory(test)
