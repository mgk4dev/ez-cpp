#pragma once

#include <ez/rpl/Chain.hpp>

namespace ez::rpl {

template <typename Range, typename... StageFactories>
auto run(Range&& range, StageFactories&&... factories)
{
    if constexpr (sizeof...(StageFactories) == 0) {
        return std::decay_t<Range>(std::forward<Range>(range));
    }
    else {
        using InputType = decltype(std::forward<Range>(range));
        auto chain = rpl::make_chain<InputType>(std::forward<StageFactories>(factories)...);
        return chain.first().process_batch(std::forward<Range>(range));
    }
}

}  // namespace ez::rpl
