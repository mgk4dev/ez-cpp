#pragma once

#include <ez/rpl/StageFactory.hpp>

#include <algorithm>

namespace ez::rpl {

template <typename InputType>
struct Sort {
    EZ_RPL_STAGE_INFO(ProcessingMode::Batch, ProcessingMode::Batch)
    using OutputType = InputType;

    template <typename Next>
    decltype(auto) process_batch(InputType input, Next&& next)
    {
        std::sort(std::begin(input), std::end(input));

        return next.process_batch(static_cast<InputType>(input));
    }
};

auto sort() { return make_factory<Sort>(); }

}  // namespace ez::rpl
