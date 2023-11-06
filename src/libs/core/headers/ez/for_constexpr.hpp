#pragma once

#include <ez/utils.hpp>

namespace ez {

template <auto begin, auto end, typename F, typename... Args>
constexpr void for_constexpr(F&& f, Args&&... args)
{
    static_assert(begin <= end, "Invalid for_constexpr indices");
    if constexpr (begin < end) {
        f(constexpr_<begin>, args...);
        for_constexpr<begin + 1, end>(f, args...);
    }
}

}  // namespace ez
