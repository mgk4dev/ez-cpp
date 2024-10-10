#pragma once

#include <ez/rpl/StageFactory.hpp>

#include <algorithm>
#include <functional>

namespace ez::rpl {

template <typename InputType, typename Less>
struct Sort {
    using OutputType = InputType;

    Less less;

    decltype(auto) process_batch(InputType input, auto&& next)
    {
        std::sort(std::begin(input), std::end(input), less);
        return next.process_batch(static_cast<InputType>(input));
    }
};

template <typename Less = std::less<>>
auto sort(Less&& less = {})
{
    return make_factory<ProcessingMode::Batch, ProcessingMode::Batch, Sort, Less>(
        std::forward<Less>(less));
}

}  // namespace ez::rpl
