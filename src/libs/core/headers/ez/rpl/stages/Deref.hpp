#pragma once

#include <ez/rpl/stages/Transform.hpp>

namespace ez::rpl {

inline auto deref()
{
    return transform([](auto&& val) -> decltype(auto) { return *EZ_FWD(val); });
}

}  // namespace ez::rpl
