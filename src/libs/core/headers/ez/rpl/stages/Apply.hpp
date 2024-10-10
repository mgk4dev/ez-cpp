#pragma once

#include <ez/rpl/stages/Transform.hpp>

namespace ez::rpl {

template <typename F>
auto apply(F&& f)
{
    return transform([f = EZ_FWD(f)](auto&& tuple) mutable -> decltype(auto) {
        return std::apply(f, EZ_FWD(tuple));
    });
}

}  // namespace ez::rpl
