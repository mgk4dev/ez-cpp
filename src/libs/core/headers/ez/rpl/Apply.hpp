#pragma once

#include <ez/rpl/StageBase.hpp>

#include <ez/Utils.hpp>

#include <functional>
#include <type_traits>

namespace ez::rpl {

template <typename F>
struct Apply : StageBase {

    F function;

    Apply(auto&& f) : function{EZ_FWD(f)} {}

    void process(auto&& val, auto&&... next)
    {
        auto func = [&](auto&& input) { return std::apply(function, EZ_FWD(val)); };

        using R = std::invoke_result_t<decltype(func), decltype(val)>;

        if constexpr (std::is_same_v<R, void>) { func(EZ_FWD(val)); }
        else {
            stage::invoke(func(EZ_FWD(val)), next...);
        }
    }
};

template <typename F>
auto apply(F&& f)
{
    return stage::make<Apply>(std::forward<F>(f));
}

}  // namespace ez::rpl
