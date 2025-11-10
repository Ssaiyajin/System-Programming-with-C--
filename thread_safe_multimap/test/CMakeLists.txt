set(TEST_PARALLEL_HT_SOURCES
   TestAtomicMultiMap.cpp
   TestMultiMap.cpp
   TestMutexMultiMap.cpp
   Tester.cpp
   )

add_executable(tester ${TEST_PARALLEL_HT_SOURCES})
target_link_libraries(tester
   parallel_ht_core
   GTest::GTest
   )
