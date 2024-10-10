#pragma once

#include <ez/rpl/Pipeline.hpp>

namespace ez::rpl {

template <typename Range, typename... StageFactories>
auto run(Range&& range, StageFactories&&... factories)
{
    if constexpr (sizeof...(StageFactories) == 0) {
        return std::decay_t<Range>(std::forward<Range>(range));
    }
    else {
        using InputType = decltype(std::forward<Range>(range));
        auto pipeline = rpl::make_pipeline<InputType>(std::forward<StageFactories>(factories)...);
        return pipeline.first().process_batch(std::forward<Range>(range));
    }
}

}  // namespace ez::rpl
