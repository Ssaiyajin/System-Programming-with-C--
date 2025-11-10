#include "lib/Find.hpp"
#include "lib/List.hpp"
#include "lib/MallocAllocator.hpp"
#include "lib/PooledAllocator.hpp"
#include <chrono>
#include <iomanip>
#include <iostream>
#include <optional>
//---------------------------------------------------------------------------
using namespace std;
using namespace pool;
//---------------------------------------------------------------------------
static constexpr size_t numInserts = 100000;
//---------------------------------------------------------------------------
template <typename Allocator>
void runBenchmark(size_t listLength, size_t numLookups, double& insert, double& lookup, double& destruct) {
    optional<List<int, Allocator>> l;
    l.emplace();
    {
        auto begin = chrono::high_resolution_clock::now();
        for (size_t i = 0; i < listLength; ++i) {
            l->insert(i + 1);
        }
        auto end = chrono::high_resolution_clock::now();
        insert = (end - begin).count();
    }
    {
        auto begin = chrono::high_resolution_clock::now();
        for (size_t i = 0; i < numLookups; ++i) {
            find(*l, static_cast<int>(listLength));
        }
        auto end = chrono::high_resolution_clock::now();
        lookup = (end - begin).count();
    }
    {
        auto begin = chrono::high_resolution_clock::now();
        l.reset();
        auto end = chrono::high_resolution_clock::now();
        destruct = (end - begin).count();
    }
}
//---------------------------------------------------------------------------
int main() {
    for (size_t listLength : {10, 100, 1000, 10000}) {
        double mallocInsert = 0.0;
        double mallocLookup = 0.0;
        double mallocDestruct = 0.0;
        double pooledInsert = 0.0;
        double pooledLookup = 0.0;
        double pooledDestruct = 0.0;

        size_t iterations = numInserts / listLength;
        size_t numLookups = numInserts / iterations;

        for (size_t i = 0; i < iterations; ++i) {
            double insert;
            double lookup;
            double destruct;

            runBenchmark<MallocAllocator<int>>(listLength, numLookups, insert, lookup, destruct);
            mallocInsert += insert;
            mallocLookup += lookup;
            mallocDestruct += destruct;

            runBenchmark<PooledAllocator<int>>(listLength, numLookups, insert, lookup, destruct);
            pooledInsert += insert;
            pooledLookup += lookup;
            pooledDestruct += destruct;
        }

        mallocInsert /= static_cast<double>(iterations) * listLength;
        mallocLookup /= static_cast<double>(iterations) * listLength * numLookups;
        mallocDestruct /= static_cast<double>(iterations) * listLength;
        pooledInsert /= static_cast<double>(iterations) * listLength;
        pooledLookup /= static_cast<double>(iterations) * listLength * numLookups;
        pooledDestruct /= static_cast<double>(iterations) * listLength;

        cout << "list length = " << setw(6) << listLength << "\n";
        cout << "\tmalloc insert:   " << setw(7) << setprecision(3) << fixed << (mallocInsert / 1000) << " us / element \n";
        cout << "\tpooled insert:   " << setw(7) << setprecision(3) << fixed << (pooledInsert / 1000) << " us / element \n";
        cout << "\tmalloc lookup:   " << setw(7) << setprecision(3) << fixed << (mallocLookup / 1000) << " us / element \n";
        cout << "\tpooled lookup:   " << setw(7) << setprecision(3) << fixed << (pooledLookup / 1000) << " us / element \n";
        cout << "\tmalloc destruct: " << setw(7) << setprecision(3) << fixed << (mallocDestruct / 1000) << " us / element \n";
        cout << "\tpooled destruct: " << setw(7) << setprecision(3) << fixed << (pooledDestruct / 1000) << " us / element \n";

        cout << endl;
    }
}
//---------------------------------------------------------------------------
