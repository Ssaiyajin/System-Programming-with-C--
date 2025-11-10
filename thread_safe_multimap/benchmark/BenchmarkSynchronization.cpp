#include "lib/AtomicMultiMap.hpp"
#include "lib/MutexMultiMap.hpp"
#include <thread>
#include <benchmark/benchmark.h>
//---------------------------------------------------------------------------
using namespace std;
using namespace hashtable;
//---------------------------------------------------------------------------
namespace {
//---------------------------------------------------------------------------
void BenchmarkMutexMultiMap(benchmark::State& state) {
    uint32_t buckets = state.range(0);
    MutexMultiMap map(buckets);

    for (auto _ : state) {
        vector<thread> threads;
        for (unsigned thread = 0; thread < 8; ++thread) {
            threads.emplace_back([&map, thread, buckets]() {
                for (uint32_t i = 0; i < 64; ++i)
                    for (uint32_t bucket = 0; bucket < buckets; ++bucket)
                        map.insert({bucket, 64 * thread + i});
            });
        }

        for (auto& thread : threads)
            thread.join();
    }

    state.SetItemsProcessed(state.iterations() * buckets * 8 * 64);
}
//---------------------------------------------------------------------------
void BenchmarkAtomicMultiMap(benchmark::State& state) {
    uint32_t buckets = state.range(0);
    AtomicMultiMap map(buckets);

    for (auto _ : state) {
        vector<thread> threads;
        for (unsigned thread = 0; thread < 8; ++thread) {
            threads.emplace_back([&map, thread, buckets]() {
                for (uint32_t i = 0; i < 64; ++i)
                    for (uint32_t bucket = 0; bucket < buckets; ++bucket)
                        map.insert({bucket, 64 * thread + i});
            });
        }

        for (auto& thread : threads)
            thread.join();
    }

    state.SetItemsProcessed(state.iterations() * buckets * 8 * 64);
}
//---------------------------------------------------------------------------
} // namespace
//---------------------------------------------------------------------------
BENCHMARK(BenchmarkMutexMultiMap)->Range(32, 4096)->RangeMultiplier(2)->UseRealTime();
BENCHMARK(BenchmarkAtomicMultiMap)->Range(32, 4096)->RangeMultiplier(2)->UseRealTime();
//---------------------------------------------------------------------------
BENCHMARK_MAIN();
//---------------------------------------------------------------------------
