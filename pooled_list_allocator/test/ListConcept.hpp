#include <concepts>
#include <cstddef>
//---------------------------------------------------------------------------
namespace pool::test {
//---------------------------------------------------------------------------
namespace impl {
struct Test {};
}; // namespace impl
//---------------------------------------------------------------------------
/// Concept for an allocator
template <typename List>
concept IsList = requires(List& l, const List& cl) {
    typename List::value_type;
    requires std::default_initializable<List>;
    requires std::movable<List>;
    { l.size() } -> std::same_as<std::size_t>;
    { cl.size() } -> std::same_as<std::size_t>;
};
//---------------------------------------------------------------------------
template <typename List>
concept CopyableList = requires(List& l) {
    requires IsList<List>;
    requires std::copyable<List>;
    { l.insert(std::declval<const typename List::value_type&>()) } -> std::same_as<typename List::value_type&>;
};
//---------------------------------------------------------------------------
template <typename List>
concept ComparableList = requires(List& l, const List& cl) {
    requires IsList<List>;
    { l.erase(std::declval<const typename List::value_type&>()) } -> std::same_as<void>;
};
//---------------------------------------------------------------------------
} // namespace pool::test
//---------------------------------------------------------------------------
