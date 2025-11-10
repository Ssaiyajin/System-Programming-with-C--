add_executable(tester Tester.cpp TestComplex.cpp TestRational.cpp)
target_link_libraries(tester arithmetic_types_core GTest::GTest)
