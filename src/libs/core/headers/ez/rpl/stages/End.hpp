#pragma once

#include <ez/rpl/StageFactory.hpp>

namespace ez::rpl {

template <typename InputType>
struct End {
    using OutputType = Unit&&;
    void process_incremental(Unit&&, auto&&) {}
    decltype(auto) flush_to(auto&& next) { return next.process_batch(Unit{}); }
};

inline auto end()
{
    return make_factory<ProcessingMode::Incremental, ProcessingMode::Batch, End>();
}
}  // namespace ez::rpl
