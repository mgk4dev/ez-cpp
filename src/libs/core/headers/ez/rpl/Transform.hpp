#pragma once

#include <ez/rpl/StageBase.hpp>

#include <ez/Utils.hpp>

#include <functional>

namespace ez::rpl {

template <typename InputType, typename F>
struct Transform : StageBase {
    using OutputType = std::invoke_result_t<F, InputType>;

    F transform;

    Transform(auto&& t) : transform{EZ_FWD(t)} {}

    void process_complete(InputType val, auto&& next)
    {
        return next.process_complete(std::invoke(transform, static_cast<InputType>(val)));
    }

    void process_incremental(InputType val, auto&& next)
    {
        return next.process_incremental(std::invoke(transform, static_cast<InputType>(val)));
    }
};

template <typename F>
auto transform(F&& f)
{
    return stage::make<Transform>(std::forward<F>(f));
}

}  // namespace ez::rpl
