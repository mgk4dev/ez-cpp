#pragma once

#include <ez/rpl/StageFactory.hpp>

namespace ez::rpl {

template <typename InputType, typename...>
struct Take {
    using OutputType = InputType;

    size_t max = 0;
    size_t count = 0;

    Take(size_t m) : max{m} {};

    decltype(auto) process_incremental(InputType input, auto&& next)
    {
        next.process_incremental(static_cast<InputType>(input));
        ++count;
    }

    bool done() const { return count == max; }
};

inline auto take(size_t count)
{
    return make_factory<ProcessingMode::Incremental, ProcessingMode::Incremental, Take>(count);
}

}  // namespace ez::rpl
