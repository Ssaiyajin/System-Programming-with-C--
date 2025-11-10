#include "lib/BinaryHeap.hpp"
#include <fstream>
#include <iostream>
//---------------------------------------------------------------------------
#include "lib/BinaryHeap.hpp"
#include <fstream>
#include <iostream>
//---------------------------------------------------------------------------
using namespace std;
using namespace binary_heap;
//---------------------------------------------------------------------------
int main(int argc, char* argv[]) {
    if (argc != 2) {
        cerr << "USAGE: " << argv[0] << " file" << endl;
        return 1;
    }


    std::ofstream out(argv[1]);

    std::vector<unsigned> heapObj; // Create a vector to represent the heap

    // Perform operations on the heapObj (insert, extract, printDot)
    unsigned value = 5; // Example value to insert
    binary_heap::insert(heapObj, value);

    // Extract a value
    unsigned extractedValue = binary_heap::extract(heapObj);
    std::cout << "Extracted value: " << extractedValue << std::endl; // Using the extracted value

    // Print the heap using printDot
    binary_heap::printDot(out, heapObj);

    return 0;
}