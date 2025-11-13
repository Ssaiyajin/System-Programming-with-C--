#ifndef H_lib_Find
#define H_lib_Find
//---------------------------------------------------------------------------
#include <concepts>
#include <iterator>
#include <type_traits>

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

// Also provide overload for containers that expose element type via iterator (fallback)
template<typename It, typename V>
requires std::equality_comparable_with<typename std::remove_reference_t<decltype(*std::declval<It>())>, V>
auto find(It beginIt, It endIt, V const& value) -> std::remove_reference_t<decltype(&*beginIt)> {
    for (auto it = beginIt; it != endIt; ++it) {
        if ((*it) == value) {
            return std::addressof(*it);
        }
    }
    return nullptr;
}

#endif // FIND_HPP
