#ifndef H_lib_GenericValue
#define H_lib_GenericValue
//---------------------------------------------------------------------------
#include <array>
#include <cstddef>
#include <cstdint>
//---------------------------------------------------------------------------
namespace hashtable {
/// A generic value that can be used in the hash table as key or value
class GenericValue {
    private:
    /// The storage of the value
    alignas(alignof(std::max_align_t)) std::array<std::byte, 64> data;

    public:
    /// Construct an empty value
    GenericValue();
    /// Destructor
    ~GenericValue();

    /// Generic values can't be copied
    GenericValue(const GenericValue&) = delete;
    /// Generic values can't be copied
    GenericValue& operator=(const GenericValue&) = delete;

    /// Move constructor
    GenericValue(GenericValue&&) noexcept;
    /// Move assignment
    GenericValue& operator=(GenericValue&&) noexcept;

    /// Get a raw pointer to the data
    std::byte* getData() { return data.data(); }
    /// Get a raw pointer to the data
    const std::byte* getData() const { return data.data(); }
};
//---------------------------------------------------------------------------
} // namespace hashtable
//---------------------------------------------------------------------------
#endif
