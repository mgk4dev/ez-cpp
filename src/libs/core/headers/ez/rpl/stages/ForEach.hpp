#pragma once

#include <ez/rpl/stages/Transform.hpp>

namespace ez::rpl {

template <typename F>
auto for_each(F&& f)
{
    return transform([f = std::forward<F>(f)](auto&& value) {
        f(EZ_FWD(value));
        return Unit{};
    });
}

}  // namespace ez::rpl
