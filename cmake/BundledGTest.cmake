find_package(GTest)

if(NOT GTEST_FOUND)
    message(STATUS "Adding bundled Google Test")

    # Turn off building GMock and installing GTest
    set(BUILD_GMOCK OFF CACHE BOOL INTERNAL)
    set(INSTALL_GTEST OFF CACHE BOOL INTERNAL)

    # Explicit binary directory required when source is outside current project
    set(GTEST_SRC_DIR "${CMAKE_CURRENT_LIST_DIR}/thirdparty/googletest")
    set(GTEST_BIN_DIR "${CMAKE_BINARY_DIR}/thirdparty/googletest_build")

    add_subdirectory("${GTEST_SRC_DIR}" "${GTEST_BIN_DIR}")

    add_library(GTest::GTest ALIAS gtest)
    add_library(GTest::Main ALIAS gtest_main)
endif()
