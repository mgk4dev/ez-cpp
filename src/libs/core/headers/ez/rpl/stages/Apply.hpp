#pragma once

#include <ez/rpl/StageFactory.hpp>

#include <ez/Utils.hpp>

#include <type_traits>

namespace ez::rpl {

template <typename InputType, typename F>
struct Apply {
    using OutputType = decltype(std::apply(std::declval<F>(), std::declval<InputType>()));

    Apply(auto&& f) : function{EZ_FWD(f)} {}

    F function;

    void process_incremental(InputType val, auto&& next)
    {
        if constexpr (std::is_same_v<OutputType, void>) {
            std::apply(function, static_cast<InputType>(val));
        }
        else {
            next.process_incremental(std::apply(function, static_cast<InputType>(val)));
        }
    }
};

template <typename F>
auto apply(F&& f)
{
    return make_factory<ProcessingMode::Incremental, ProcessingMode::Incremental, Apply, F>(
        std::forward<F>(f));
}

}  // namespace ez::rpl
