#pragma once

#include <type_traits>

namespace ez {

template <int begin, int end, typename F, typename... Args>
constexpr void for_constexpr(F&& f, Args&&... args)
{
    static_assert(begin <= end, "Invalid for_constexpr indices");
    if constexpr (begin < end) {
        f(std::integral_constant<int, begin>{}, args...);
        for_constexpr<begin + 1, end>(f, args...);
    }
}

}  // namespace ez
