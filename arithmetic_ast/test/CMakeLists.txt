add_executable(tester Tester.cpp TestAST.cpp TestPrintVisitor.cpp)
target_link_libraries(tester ast_core GTest::GTest)
