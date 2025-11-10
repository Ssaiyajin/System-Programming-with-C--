add_executable(tester Tester.cpp TestSortPointers.cpp)
target_link_libraries(tester sort_pointers_core GTest::GTest)