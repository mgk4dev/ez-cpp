#pragma once

#include <ranges>

namespace ez::rpl {

template <typename T>
inline auto iota(T&& start)
{
    return std::ranges::views::iota(std::forward<T>(start));
}

template <typename T, typename U>
inline auto iota(T&& start, U&& end)
{
    return std::ranges::views::iota(std::forward<T>(start), std::forward<U>(end));
}

}  // namespace ez::rpl
