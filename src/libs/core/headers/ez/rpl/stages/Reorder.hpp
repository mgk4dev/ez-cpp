#pragma once

#include <ez/rpl/StageFactory.hpp>

#include <ez/Tuple.hpp>

namespace ez::rpl {

template <typename InputType, size_t... indices>
struct ReorderImpl {
    using OutputTuple = Tuple<decltype(std::get<indices>(std::declval<InputType>()))...>;
    using OutputType = OutputTuple&&;

    void process_incremental(InputType input, auto&& next)
    {
        next.process_incremental(OutputTuple{std::get<indices>(static_cast<InputType>(input))...});
    }
};

template <size_t... indices>
struct Reorder {
    template <typename InputType>
    using Stage = ReorderImpl<InputType, indices...>;
};

template <size_t... indices>
auto reorder()
{
    return make_factory<ProcessingMode::Incremental, ProcessingMode::Incremental,
                        Reorder<indices...>::template Stage>();
}

}  // namespace ez::rpl
