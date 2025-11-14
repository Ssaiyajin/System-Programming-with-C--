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
void printDot(std::ostream& out, const std::vector<unsigned>& heap);
//---------------------------------------------------------------------------
} // namespace binary_heap
//---------------------------------------------------------------------------
#endif
//---------------------------------------------------------------------------