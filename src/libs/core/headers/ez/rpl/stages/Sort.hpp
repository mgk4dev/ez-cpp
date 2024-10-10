#pragma once

#include <ez/rpl/stages/Transform.hpp>

#include <algorithm>

namespace ez::rpl {

template <typename Less = std::less<>>
auto sort(Less&& less = {})
{
    return transform_batch([less = std::forward<Less>(less)](auto&& range) -> decltype(auto) {
        std::sort(std::begin(range), std::end(range), less);
        return EZ_FWD(range);
    });
}

}  // namespace ez::rpl
