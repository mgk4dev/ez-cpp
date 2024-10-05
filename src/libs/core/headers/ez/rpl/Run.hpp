#pragma once

#include <ez/rpl/Chain.hpp>

#include <tuple>

namespace ez::rpl {

template <typename Range, typename... StageFactories>
decltype(auto) run(Range&& range, StageFactories&&... factories)
{
    if constexpr (sizeof...(StageFactories) == 0) {
        return std::decay_t<Range>(std::forward<Range>(range));
    }
    else {
        using InputType = decltype(std::forward<Range>(range));

        Chain<InputType, std::decay_t<StageFactories>...> chain{
            std::forward<StageFactories>(factories)...};

        return 2121;

        //return chain.get(Index<0>()).process_all(std::forward<Range>(range));
    }
}

}  // namespace ez::rpl
