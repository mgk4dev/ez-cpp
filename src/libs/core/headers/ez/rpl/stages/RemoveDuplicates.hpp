#pragma once

#include <ez/rpl/stages/Transform.hpp>

#include <algorithm>

namespace ez::rpl {

template <typename Equals = std::equal_to<>>
auto remove_duplicates(Equals&& equals = {})
{
    return transform_batch([equals = std::forward<Equals>(equals)](auto&& range) -> decltype(auto) {
        auto last = std::unique(std::begin(range), std::end(range), equals);
        range.erase(last, range.end());
        return EZ_FWD(range);
    });
}

}  // namespace ez::rpl
