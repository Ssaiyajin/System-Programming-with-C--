set(PARALLEL_HT_CORE_SOURCES
   AtomicMultiMap.cpp
   MultiMap.cpp
   MutexMultiMap.cpp
   )

add_library(parallel_ht_core ${PARALLEL_HT_CORE_SOURCES})
target_include_directories(parallel_ht_core PUBLIC ${CMAKE_SOURCE_DIR})

add_clang_tidy_target(lint_parallel_ht_core ${PARALLEL_HT_CORE_SOURCES})
add_dependencies(lint lint_parallel_ht_core)
