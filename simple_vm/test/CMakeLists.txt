add_executable(tester tester.cpp test_simplevm.cpp)
target_link_libraries(tester simplevm_core GTest::GTest)
