#pragma once

#include <ez/rpl/StageBase.hpp>

#include <ez/Utils.hpp>

#include <functional>

namespace ez::rpl {

template <typename F>
struct ForEach : StageBase {
    F function;

    ForEach(auto&& f) : function{EZ_FWD(f)} {}

    void process(auto&& val, auto&&... next)
    {
        forward(std::invoke(function, EZ_FWD(val)), next...);
    }
};

template <typename F>
auto for_each(F&& f)
{
    return stage::make<ForEach>(std::forward<F>(f));
}

}  // namespace ez::rpl
