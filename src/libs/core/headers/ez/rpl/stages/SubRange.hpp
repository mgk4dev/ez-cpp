#pragma once

#include <ez/rpl/stages/Transform.hpp>

#include <ranges>

namespace ez::rpl {

inline auto sub_range(auto&& lower, auto&& upper)
{
    return transform_batch([&](auto&& collection) {
        return std::ranges::subrange(std::ranges::lower_bound(EZ_FWD(collection), lower),
                                     std::ranges::upper_bound(EZ_FWD(collection), upper));
    });
}

}  // namespace ez::rpl
