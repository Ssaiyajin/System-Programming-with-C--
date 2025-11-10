add_executable(tester Tester.cpp TestBitSet.cpp)
target_link_libraries(tester bitset_core GTest::GTest)
