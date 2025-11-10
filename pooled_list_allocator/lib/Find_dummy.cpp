#include "lib/Find.hpp"
#include "lib/List.hpp"
#include "lib/MallocAllocator.hpp"
#include <vector>
//---------------------------------------------------------------------------
void allocatorListDummyFind(std::vector<int>& v, pool::List<int, pool::MallocAllocator<int>>& l) {
    static_cast<void>(pool::find(v, 124));
    static_cast<void>(pool::find(l, 123));
}
//---------------------------------------------------------------------------
