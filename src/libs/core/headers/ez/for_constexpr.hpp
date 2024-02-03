#pragma once

#include <ez/utils.hpp>

namespace ez {

/// Compile time for loops.
/// Usage
/// @code
/// auto tuple = std::make_tuple(10, 10, 10);
/// for_constexpr<0, 3>([&](auto index) { std::get<index.value>(tuple) = index.value; });
/// @endcode
///
template <auto begin, auto end, typename F, typename... Args>
constexpr void for_constexpr(F&& f, Args&&... args)
{
    static_assert(begin <= end, "Invalid for_constexpr indices");
    if constexpr (begin < end) {
        f(EZ_CONSTEXP(begin), args...);
        for_constexpr<begin + 1, end>(f, args...);
    }
}

}  // namespace ez
