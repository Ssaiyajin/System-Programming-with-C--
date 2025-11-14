#ifndef H_lib_BinaryHeap
#define H_lib_BinaryHeap
//---------------------------------------------------------------------------
#include <ostream>
#include <vector>
//---------------------------------------------------------------------------
namespace binary_heap {
//---------------------------------------------------------------------------
/// Insert a value into the heap
void insert(std::vector<unsigned>& heap, unsigned value);
//---------------------------------------------------------------------------
/// Extract a value from the heap
unsigned extract(std::vector<unsigned>& heap);
//---------------------------------------------------------------------------
/// Print a dotfile representing the heap
inline void printDot(std::ostream &out, const std::vector<unsigned> &heap)
{
    out << "digraph {\n";

    // print node labels (keep same ordering as tests)
    for (size_t i = 0; i < heap.size(); ++i) {
        out << '\t' << i << " [label=\"" << heap[i] << "\"];\n";
    }

    // print edges using pre-order DFS so edges appear in the expected order:
    // visit node -> left child, recurse left subtree, then node -> right child, recurse right subtree
    std::function<void(size_t)> dfs = [&](size_t idx) {
        size_t left  = 2 * idx + 1;
        size_t right = 2 * idx + 2;

        if (left < heap.size()) {
            out << '\t' << idx << " -> " << left << ";\n";
            dfs(left);
        }
        if (right < heap.size()) {
            out << '\t' << idx << " -> " << right << ";\n";
            dfs(right);
        }
    };

    if (!heap.empty())
        dfs(0);

    out << "}\n";
}
//---------------------------------------------------------------------------
} // namespace binary_heap
//---------------------------------------------------------------------------
#endif
//---------------------------------------------------------------------------