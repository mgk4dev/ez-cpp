#pragma once

#include <ez/rpl/stages/Transform.hpp>

namespace ez::rpl {

template <size_t index>
auto get()
{
    return transform([](auto&& tuple) -> decltype(auto) { return std::get<index>(EZ_FWD(tuple)); });
}

}  // namespace ez::rpl
