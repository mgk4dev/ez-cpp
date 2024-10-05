#pragma once

#include <ez/rpl/StageFactory.hpp>

#include <functional>
#include <type_traits>

namespace ez::rpl {

template <typename InputType, typename F>
struct Transform {
    EZ_RPL_STAGE_INFO(ProcessingStyle::Incremental, ProcessingStyle::Incremental)
    using OutputType = decltype(std::declval<F&>(std::declval<InputType>()));

    F f;

    template <typename Next>
    void process_incremental(InputType input, Next&& next)
    {
        next.process_incremental(std::invoke(f, static_cast<InputType>(input)));
    }
};

template <typename InputType, typename F>
struct TransformAll {

    EZ_RPL_STAGE_INFO(ProcessingStyle::All, ProcessingStyle::All)


    using InvokeResult = std::invoke_result_t<F&, decltype(std::declval<InputType>())>;
    using OutputType = decltype(std::forward<InvokeResult>(std::declval<InvokeResult>()));
    F f;

    template <typename Next>
    decltype(auto) process_all(InputType input, Next&& next)
    {
        return next.process_all(f(static_cast<InputType>(input)));
    }
};

template <typename F>
auto transform(F&& f)
{
    return make_factory<Transform, F>(std::forward<F>(f));
}

template <typename F>
auto transform_all(F&& f)
{
    return make_factory<TransformAll, F>(std::forward<F>(f));
}

}  // namespace ez::rpl
