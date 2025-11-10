#include "lib/GenericValue.hpp"
#include <cstring>
//---------------------------------------------------------------------------
using namespace std;
//---------------------------------------------------------------------------
namespace hashtable {
//---------------------------------------------------------------------------
GenericValue::GenericValue()
// Construct an empty entry
{
    memset(data.data(), 0, sizeof(data));
}
//---------------------------------------------------------------------------
GenericValue::~GenericValue()
// Destructor
{
}
//---------------------------------------------------------------------------
GenericValue::GenericValue(GenericValue&& other) noexcept
    : data(other.data)
// Move constructor
{
    // Invalidate other by setting its data to 0xcc
    memset(other.data.data(), 0xcc, sizeof(other.data));
}
//---------------------------------------------------------------------------
GenericValue& GenericValue::operator=(GenericValue&& other) noexcept
// Move assignment
{
    if (this == &other)
        return *this;

    data = other.data;
    // Invalidate other by setting its data to 0xcc
    memset(other.data.data(), 0xcc, sizeof(other.data));

    return *this;
}
//---------------------------------------------------------------------------
} // namespace hashtable
//---------------------------------------------------------------------------
