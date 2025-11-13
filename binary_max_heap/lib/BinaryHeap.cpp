#include "BinaryHeap.hpp"
#include <vector>
#include <iostream>
#include <stdexcept>
#include <algorithm>

namespace binary_heap {

//---------------------------------------------------------------------------
// Insert a value into the heap
void insert(std::vector<unsigned>& heap, unsigned value) {
    heap.push_back(value); // Add to the end
    int index = heap.size() - 1;

    // Bubble up
    while (index > 0) {
        int parent = (index - 1) / 2;
        if (heap[index] > heap[parent]) {
            std::swap(heap[index], heap[parent]);
            index = parent;
        } else {
            break;
        }
    }
}

//---------------------------------------------------------------------------
// Extract the maximum value from the heap
unsigned extract(std::vector<unsigned>& heap) {
    if (heap.empty()) {
        throw std::runtime_error("Heap is empty");
    }

    unsigned root = heap[0];
    heap[0] = heap.back();
    heap.pop_back();

    int index = 0;
    int heapSize = heap.size();

    // Bubble down
    while (true) {
        int largest = index;
        int leftChild = 2 * index + 1;
        int rightChild = 2 * index + 2;

        if (leftChild < heapSize && heap[leftChild] > heap[largest])
            largest = leftChild;

        if (rightChild < heapSize && heap[rightChild] > heap[largest])
            largest = rightChild;

        if (largest != index) {
            std::swap(heap[index], heap[largest]);
            index = largest;
        } else {
            break;
        }
    }

    return root;
}

//---------------------------------------------------------------------------
// Print heap as a dot graph for visualization
void printDot(std::ostream& out, const std::vector<unsigned>& heap) {
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

        if (leftChild < heap.size()) out << "\t" << i << " -> " << leftChild << ";\n";
        if (rightChild < heap.size()) out << "\t" << i << " -> " << rightChild << ";\n";
    }

    out << "}\n";
}

} // namespace binary_heap
