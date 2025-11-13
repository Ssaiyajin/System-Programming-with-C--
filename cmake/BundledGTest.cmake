find_package(benchmark)

if (NOT benchmark_FOUND)
    message(STATUS "Adding bundled Google Benchmark")

    set(BENCHMARK_SRC_DIR "${CMAKE_CURRENT_LIST_DIR}/thirdparty/benchmark")

    if (EXISTS "${BENCHMARK_SRC_DIR}/CMakeLists.txt")
        # Use explicit binary dir when called out-of-tree
        set(BENCHMARK_BIN_DIR "${CMAKE_BINARY_DIR}/_benchmark_build")
        add_subdirectory("${BENCHMARK_SRC_DIR}" "${BENCHMARK_BIN_DIR}")

        if (TARGET benchmark)
            add_library(benchmark::benchmark ALIAS benchmark)
        else()
            message(WARNING "benchmark target not found after add_subdirectory!")
        endif()
    else()
        message(FATAL_ERROR "Bundled Google Benchmark source not found at ${BENCHMARK_SRC_DIR}")
    endif()
endif()
