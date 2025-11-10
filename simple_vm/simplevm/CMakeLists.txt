add_library(simplevm_core simplevm.cpp)
target_include_directories(simplevm_core PUBLIC ${CMAKE_SOURCE_DIR})

add_clang_tidy_target(lint_simplevm_core simplevm.cpp)
add_dependencies(lint lint_simplevm_core)

add_executable(simplevm main.cpp)
target_link_libraries(simplevm PUBLIC simplevm_core)
