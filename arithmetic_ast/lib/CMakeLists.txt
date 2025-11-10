add_library(ast_core AST.cpp EvaluationContext.cpp PrintVisitor.cpp)
target_include_directories(ast_core PUBLIC ${CMAKE_SOURCE_DIR})

add_clang_tidy_target(lint_ast_core AST.cpp EvaluationContext.cpp PrintVisitor.cpp)
add_dependencies(lint lint_ast_core)
