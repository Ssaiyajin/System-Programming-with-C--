#include "lib/SortPointers.hpp"
#include <cassert>
#include <vector>
//---------------------------------------------------------------------------
using namespace std;
//---------------------------------------------------------------------------
namespace sort_pointers {
//---------------------------------------------------------------------------
void quicksort(const unsigned** begin, const unsigned** end) {
    // Base case: if the range has 0 or 1 elements, it's already sorted
    if (end - begin <= 1) {
        return;
    }

    // Choose a pivot element (for example, the middle element)
    const unsigned* pivotValue = *begin;
    const unsigned** left = begin + 1;
    const unsigned** right = end - 1;

    while (left <= right) {
        while (left <= right && **left <= *pivotValue) {
            ++left;
        }
        while (left <= right && **right > *pivotValue) {
            --right;
        }
        if (left < right) {
            std::swap(*left, *right);
        }
    }

    // Move the pivot element to its correct position
    std::swap(*begin, *right);

    // Recursively sort the two partitions
    quicksort(begin, right);
    quicksort(right + 1, end);
}

//---------------------------------------------------------------------------
void merge(const unsigned** begin, const unsigned** mid, const unsigned** end);

void mergesort(const unsigned** begin, const unsigned** end) {
    if (end - begin <= 1) {
        return;
    }

    const unsigned** mid = begin + (end - begin) / 2;

    mergesort(begin, mid);
    mergesort(mid, end);

    merge(begin, mid, end);
}

void merge(const unsigned** begin, const unsigned** mid, const unsigned** end) {
    std::vector<const unsigned*> temp(end - begin);

    const unsigned** left = begin;
    const unsigned** right = mid;
    size_t index = 0;

    while (left < mid && right < end) {
        if (**left < **right) {  // Compare the values pointed by *left and *right
            temp[index++] = *left++;
        } else {
            temp[index++] = *right++;
        }
    }

    while (left < mid) {
        temp[index++] = *left++;
    }

    while (right < end) {
        temp[index++] = *right++;
    }

    for (size_t i = 0; i < temp.size(); ++i) {
        *(begin + i) = temp[i];
    }
}
//---------------------------------------------------------------------------
} // namespace sort_pointers
//---------------------------------------------------------------------------
