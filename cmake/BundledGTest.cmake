find_package(GTest)

if (NOT GTest_FOUND)
    message(STATUS "Adding bundled Google Test")

    # Disable installation and gmock
    set(BUILD_GMOCK OFF CACHE BOOL "Disable building gmock" FORCE)
    set(INSTALL_GTEST OFF CACHE BOOL "Disable installation of gtest" FORCE)

    # Make sure path exists before adding
    set(GTEST_SRC_DIR "${CMAKE_CURRENT_LIST_DIR}/thirdparty/googletest")

    if (EXISTS "${GTEST_SRC_DIR}/CMakeLists.txt")
        # Use explicit binary dir when called out of tree
        set(GTEST_BIN_DIR "${CMAKE_BINARY_DIR}/_googletest_build")
        add_subdirectory("${GTEST_SRC_DIR}" "${GTEST_BIN_DIR}")

        if (TARGET gtest)
            add_library(GTest::GTest ALIAS gtest)
        else()
            message(WARNING "GTest::GTest target not found after add_subdirectory!")
        endif()
    else()
        message(FATAL_ERROR "Bundled GoogleTest source not found at ${GTEST_SRC_DIR}")
    endif()
endif()
