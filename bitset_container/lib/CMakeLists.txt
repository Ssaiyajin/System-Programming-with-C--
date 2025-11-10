add_library(bitset_core BitSet.cpp)
target_include_directories(bitset_core PUBLIC ${CMAKE_SOURCE_DIR})

add_clang_tidy_target(lint_bitset_core BitSet.cpp)
add_dependencies(lint lint_bitset_core)
