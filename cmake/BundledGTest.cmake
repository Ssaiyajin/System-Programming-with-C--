find_package(GTest)

if (NOT GTest_FOUND)
    message(STATUS "Adding bundled Google Test")

    set(GTEST_SRC_DIR "${CMAKE_CURRENT_LIST_DIR}/thirdparty/googletest")

    if (EXISTS "${GTEST_SRC_DIR}/CMakeLists.txt")
        set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)
        set(BUILD_GMOCK OFF CACHE BOOL "" FORCE)

        # Specify a binary directory explicitly (required for out-of-tree builds)
        set(GTEST_BIN_DIR "${CMAKE_BINARY_DIR}/_gtest_build")
        add_subdirectory("${GTEST_SRC_DIR}" "${GTEST_BIN_DIR}")

        # Ensure alias targets exist regardless of upstream naming
        if (TARGET gtest)
            add_library(GTest::GTest ALIAS gtest)
        elseif (TARGET GTest::gtest)
            add_library(GTest::GTest ALIAS GTest::gtest)
        elseif (TARGET gtest_main)
            add_library(GTest::GTest ALIAS gtest_main)
        else()
            message(WARNING "No valid gtest target found after building bundled GoogleTest.")
        endif()
    else()
        message(FATAL_ERROR "Bundled Google Test source not found at ${GTEST_SRC_DIR}")
    endif()
endif()
