#include "lib/BinaryHeap.hpp"
#include <cassert>
//---------------------------------------------------------------------------
using namespace std;
//---------------------------------------------------------------------------
namespace binary_heap {
//---------------------------------------------------------------------------
std::vector<unsigned> heap; // This vector stores the heap elements

void insert(unsigned value) {
    heap.push_back(value); // Add the value to the bottom of the heap
    int index = heap.size() - 1; // Index of the newly inserted element

    while (index > 0) {
        int parent = (index - 1) / 2; // Find the parent index

        if (heap[index] > heap[parent]) {
            std::swap(heap[index], heap[parent]); // Swap if the added element is greater than its parent
            index = parent; // Move up the heap
        } else {
            break; // Stop if the correct order is reached
        }
    }
}
    
//---------------------------------------------------------------------------
unsigned extract() {
    if (heap.empty()) {
        throw std::runtime_error("Heap is empty");
    }

    unsigned root = heap[0]; // Store the value of the root to be returned
    heap[0] = heap.back(); // Replace root with the last element
    heap.pop_back(); // Remove the last element

    int index = 0;
    int heapSize = heap.size();

    while (true) {
        int largest = index;
        int leftChild = 2 * index + 1;
        int rightChild = 2 * index + 2;

        // Compare with left child
        if (leftChild < heapSize && heap[leftChild] > heap[largest]) {
            largest = leftChild;
        }

        // Compare with right child
        if (rightChild < heapSize && heap[rightChild] > heap[largest]) {
            largest = rightChild;
        }

        if (largest != index) {
            std::swap(heap[index], heap[largest]); // Swap with the larger child
            index = largest; // Move down the heap
        } else {
            break; // Stop if the correct order is reached
        }
    }

    return root; // Return the extracted root value
}
//---------------------------------------------------------------------------
void printDot(ostream& out, const vector<unsigned>& heap) {
    if (heap.empty()) {
        out << "digraph {\n}\n";
        return;
    }

    out << "digraph {\n";

    for (size_t i = 0; i < heap.size(); ++i) {
        out << "\t" << i << " [label=\"" << heap[i] << "\"];\n";
    }

    for (size_t i = 0; i < heap.size(); ++i) {
        size_t leftChild = 2 * i + 1;
        size_t rightChild = 2 * i + 2;

        if (leftChild < heap.size()) {
            out << "\t" << i << " -> " << leftChild << ";\n";
        }

        if (rightChild < heap.size()) {
            out << "\t" << i << " -> " << rightChild << ";\n";
        }
    }

    out << "}\n";
}

//---------------------------------------------------------------------------
} // namespace binary_heap
//---------------------------------------------------------------------------
