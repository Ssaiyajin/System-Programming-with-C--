add_executable(parallel_ht_benchmark BenchmarkSynchronization.cpp)
target_link_libraries(parallel_ht_benchmark
   parallel_ht_core
   benchmark)