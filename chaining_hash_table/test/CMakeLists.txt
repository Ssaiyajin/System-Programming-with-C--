add_executable(tester Tester.cpp TestChainingHashTable.cpp)
target_link_libraries(tester chaining_ht_core GTest::GTest)
