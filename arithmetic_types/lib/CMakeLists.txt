add_library(arithmetic_types_core Complex.cpp Rational.cpp)
target_include_directories(arithmetic_types_core PUBLIC ${CMAKE_SOURCE_DIR})

add_clang_tidy_target(lint_arithmetic_types_core Complex.cpp Rational.cpp)
add_dependencies(lint lint_arithmetic_types_core)
