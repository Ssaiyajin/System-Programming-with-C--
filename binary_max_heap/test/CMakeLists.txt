add_executable(tester Tester.cpp TestBinaryHeap.cpp)
target_link_libraries(tester binary_heap_core GTest::GTest)
