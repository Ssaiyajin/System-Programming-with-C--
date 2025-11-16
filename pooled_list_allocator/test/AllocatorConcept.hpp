#include <concepts>
#include <utility>
//---------------------------------------------------------------------------
namespace pool::test {
//---------------------------------------------------------------------------
namespace impl {
struct AllocatorTestType {};
}; // namespace impl
//---------------------------------------------------------------------------
/// Concept for an allocator
template <typename Allocator>
concept IsAllocator = requires(Allocator& a) {
    typename Allocator::value_type;
    typename Allocator::template rebind<typename Allocator::value_type>;
    typename Allocator::template rebind<impl::AllocatorTestType>;
    requires std::same_as<typename Allocator::template rebind<typename Allocator::value_type>, Allocator>;
    requires !std::same_as<typename Allocator::template rebind<impl::AllocatorTestType>, Allocator>;
    requires std::default_initializable<Allocator>;
    requires std::movable<Allocator>;
    { a.allocate() } -> std::same_as<typename Allocator::value_type*>;
    { a.deallocate(std::declval<typename Allocator::value_type*>()) } -> std::same_as<void>;
};
//---------------------------------------------------------------------------
} // namespace pool::test
//---------------------------------------------------------------------------
