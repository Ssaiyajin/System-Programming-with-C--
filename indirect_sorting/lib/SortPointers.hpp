#ifndef H_lib_SortPointers
#define H_lib_SortPointers
//---------------------------------------------------------------------------
namespace sort_pointers {
//---------------------------------------------------------------------------
/// Sort the range of pointers [begin; end)
void quicksort(const unsigned** begin, const unsigned** end);
//---------------------------------------------------------------------------
/// Sort the range of pointers [begin; end)
void mergesort(const unsigned** begin, const unsigned** end);
//---------------------------------------------------------------------------
} // namespace sort_pointers
//---------------------------------------------------------------------------
#endif
//---------------------------------------------------------------------------
