#pragma once

#include <ez/rpl/StageFactory.hpp>

#include <functional>
#include <type_traits>

namespace ez::rpl {

template <typename InputType, typename T>
struct Transform {
    using OutputType = std::add_rvalue_reference_t<std::invoke_result_t<T&, InputType>>;

    T transform;

    Transform(auto&& t) : transform{EZ_FWD(t)} {}

    void process_incremental(InputType input, auto&& next)
    {
        next.process_incremental(std::invoke(transform, static_cast<InputType>(input)));
    }
};

template <typename InputType, typename T>
struct TransformBatch {
    using OutputType = InputType;

    T transform;

    TransformBatch(auto&& t) : transform{EZ_FWD(t)} {}

    decltype(auto) process_batch(InputType input, auto&& next)
    {
        return next.process_batch(std::invoke(transform, static_cast<InputType>(input)));
    }
};

template <typename T>
auto transform(T&& t)
{
    return make_factory<ProcessingMode::Incremental, ProcessingMode::Incremental, Transform, T>(
        std::forward<T>(t));
}

template <typename T>
auto transform_batch(T&& t)
{
    return make_factory<ProcessingMode::Batch, ProcessingMode::Batch, TransformBatch, T>(
        std::forward<T>(t));
}

}  // namespace ez::rpl
