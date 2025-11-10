add_executable(tester_a Tester.cpp TestMallocAllocator.cpp)
target_include_directories(tester_a PRIVATE ${CMAKE_SOURCE_DIR})
target_link_libraries(tester_a allocator_list_core GTest::GTest)

add_executable(tester_b Tester.cpp TestPooledAllocator.cpp)
target_include_directories(tester_b PRIVATE ${CMAKE_SOURCE_DIR})
target_link_libraries(tester_b allocator_list_core GTest::GTest)

add_executable(tester_c Tester.cpp TestList.cpp)
target_include_directories(tester_c PRIVATE ${CMAKE_SOURCE_DIR})
target_link_libraries(tester_c allocator_list_core GTest::GTest)

add_executable(tester_d Tester.cpp TestFind.cpp)
target_include_directories(tester_d PRIVATE ${CMAKE_SOURCE_DIR})
target_link_libraries(tester_d allocator_list_core GTest::GTest)
