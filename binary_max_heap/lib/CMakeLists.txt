add_library(binary_heap_core BinaryHeap.cpp)
target_include_directories(binary_heap_core PUBLIC ${CMAKE_SOURCE_DIR})

add_clang_tidy_target(lint_binary_heap_core BinaryHeap.cpp)
add_dependencies(lint lint_binary_heap_core)
