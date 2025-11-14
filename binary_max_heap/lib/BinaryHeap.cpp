#include "BinaryHeap.hpp"

#include <algorithm>
#include <iostream>
#include <stdexcept>
#include <vector>
#include <functional>

namespace binary_heap {

// Insert a value into the max-heap.
// Complexity: O(log n)
void insert(std::vector<unsigned>& heap, unsigned value) noexcept {
    heap.push_back(value);
    std::size_t idx = heap.size() - 1;

    while (idx > 0) {
        std::size_t parent = (idx - 1) / 2;
        if (heap[idx] <= heap[parent])
            break;
        std::swap(heap[idx], heap[parent]);
        idx = parent;
    }
}

// Extract the maximum value from the heap.
// Throws std::runtime_error when heap is empty.
// Complexity: O(log n)
unsigned extract(std::vector<unsigned>& heap) {
    if (heap.empty()) {
        throw std::runtime_error("extract from empty heap");
    }

    unsigned root = heap.front();

    if (heap.size() == 1) {
        heap.pop_back();
        return root;
    }

    // Move last element to root and restore heap property.
    heap[0] = heap.back();
    heap.pop_back();

    std::size_t idx = 0;
    const std::size_t n = heap.size();

    while (true) {
        std::size_t left = 2 * idx + 1;
        std::size_t right = 2 * idx + 2;
        std::size_t largest = idx;

        if (left < n && heap[left] > heap[largest]) largest = left;
        if (right < n && heap[right] > heap[largest]) largest = right;

        if (largest == idx) break;

        std::swap(heap[idx], heap[largest]);
        idx = largest;
    }

    return root;
}

// Print heap as a dot graph. The output ordering of edges follows a
// pre-order traversal where for each node we print the left edge and
// descend the left subtree, then print the right edge and descend the
// right subtree. This ordering matches the expectations in the tests.
void printDot(std::ostream& out, const std::vector<unsigned>& heap) {
    out << "digraph {\n";

    // Node labels (index -> value)
    for (std::size_t i = 0; i < heap.size(); ++i) {
        out << '\t' << i << " [label=\"" << heap[i] << "\"];\n";
    }

    // Recursive pre-order traversal that prints an edge to the left child,
    // recurses left, then prints an edge to the right child and recurses right.
    if (!heap.empty()) {
        std::function<void(std::size_t)> dfs = [&](std::size_t idx) {
            std::size_t left = 2 * idx + 1;
            std::size_t right = 2 * idx + 2;

            if (left < heap.size()) {
                out << '\t' << idx << " -> " << left << ";\n";
                dfs(left);
            }
            if (right < heap.size()) {
                out << '\t' << idx << " -> " << right << ";\n";
                dfs(right);
            }
        };

        dfs(0);
    }

    out << "}\n";
}

} // namespace binary_heap