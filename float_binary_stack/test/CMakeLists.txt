add_executable(tester Tester.cpp TestObjectRepresentation.cpp)
target_link_libraries(tester object_representation_core GTest::GTest)
