#include "lib/BinaryHeap.hpp"
#include <cassert>
//---------------------------------------------------------------------------
using namespace std;
//---------------------------------------------------------------------------
namespace binary_heap {

void insert(std::vector<unsigned>& heap, unsigned value) {
    heap.push_back(value);
    int index = heap.size() - 1;
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

unsigned extract(std::vector<unsigned>& heap) {
    if (heap.empty()) throw std::runtime_error("Heap is empty");

    unsigned root = heap[0];
    heap[0] = heap.back();
    heap.pop_back();

    int index = 0;
    int heapSize = heap.size();

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
        } else break;
    }

    return root;
}
