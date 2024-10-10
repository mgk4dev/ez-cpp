#pragma once

#include <ez/rpl/StageFactory.hpp>

namespace ez::rpl {

template <typename InputType, typename...>
struct Count {
    using OutputType = size_t&&;

    size_t count = 0;

    decltype(auto) process_incremental(InputType, auto&& next) { ++count; }

    decltype(auto) flush_to(auto&& next) { return next.process_batch(count); }
};

inline auto count()
{
    return make_factory<ProcessingMode::Incremental, ProcessingMode::Batch, Count>();
}

}  // namespace ez::rpl
