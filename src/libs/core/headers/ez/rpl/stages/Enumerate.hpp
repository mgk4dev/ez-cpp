#pragma once

#include <ez/rpl/stages/Transform.hpp>

#include <ez/Tuple.hpp>

namespace ez::rpl {

inline auto enumerate()
{
    return transform([index = size_t{0}](auto&& input) mutable -> decltype(auto) {
        return Tuple<size_t, decltype(input)>(index++, EZ_FWD(input));
    });
}

}  // namespace ez::rpl
