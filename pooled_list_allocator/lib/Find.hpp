#ifndef H_lib_Find
#define H_lib_Find
//---------------------------------------------------------------------------
#include <concepts>
#include <iterator>
#include <type_traits>
#include <utility>

namespace pool {

// find for containers exposing value_type and supporting std::begin/std::end
template<typename Container, typename V>
requires std::equality_comparable_with<typename Container::value_type, V>
auto find(Container& c, V const& value) -> typename Container::value_type* {
    for (auto it = std::begin(c); it != std::end(c); ++it) {
        if ((*it) == value) {
            return std::addressof(*it);
        }
    }
    return nullptr;
}

// range overload: takes two iterators
template<typename It, typename V>
requires std::equality_comparable_with<std::remove_reference_t<decltype(*std::declval<It>())>, V>
auto find(It beginIt, It endIt, V const& value) -> std::remove_pointer_t<decltype(std::addressof(*beginIt))> {
    for (auto it = beginIt; it != endIt; ++it) {
        if ((*it) == value) {
            return std::addressof(*it);
        }
    }
    return nullptr;
}

} // namespace pool

#endif // POOL_FIND_HPP