#pragma once

#include <ez/rpl/StageFactory.hpp>

#include <ez/Option.hpp>

namespace ez::rpl {

template <typename InputType, typename Less>
struct Min {
    using R = Option<std::remove_cvref_t<InputType>>;
    using OutputType = R&&;

    Less less;

    R result;

    void process_incremental(InputType input, auto&&)
    {
        if (!result || less(std::as_const(input), result)) result = static_cast<InputType>(input);
    }

    decltype(auto) flush_to(auto&& next) { return next.process_batch(std::move(result)); }
};

template <typename InputType, typename Greater>
struct Max {
    using R = Option<std::remove_cvref_t<InputType>>;
    using OutputType = R&&;

    Greater greater;

    R result;

    void process_incremental(InputType input, auto&&)
    {
        if (!result || greater(std::as_const(input), result))
            result = static_cast<InputType>(input);
    }

    decltype(auto) flush_to(auto&& next) { return next.process_batch(std::move(result)); }
};

template <typename Less = std::less<>>
inline auto min(Less&& less = {})
{
    return make_factory<ProcessingMode::Incremental, ProcessingMode::Batch, Min, Less>(
        std::forward<Less>(less));
}

template <typename Greater = std::greater<>>
inline auto max(Greater&& greater = {})
{
    return make_factory<ProcessingMode::Incremental, ProcessingMode::Batch, Max, Greater>(
        std::forward<Greater>(greater));
}

}  // namespace ez::rpl
