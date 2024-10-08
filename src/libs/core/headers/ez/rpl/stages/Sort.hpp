#pragma once

#include <ez/rpl/StageFactory.hpp>

#include <algorithm>

namespace ez::rpl {

template <typename InputType, typename Less>
struct Sort {
    EZ_RPL_STAGE_INFO(ProcessingMode::Batch, ProcessingMode::Batch)
    using OutputType = InputType;


    Less less;

    template <typename Next>
    decltype(auto) process_batch(InputType input, Next&& next)
    {
        std::sort(std::begin(input), std::end(input), less);

        return next.process_batch(static_cast<InputType>(input));
    }
};

template <typename Less = std::less<>>
auto sort(Less&& less = {}) { return make_factory<Sort, Less>(std::forward<Less>(less)); }

}  // namespace ez::rpl
