#pragma once

#include <ez/rpl/Compose.hpp>
#include <ez/rpl/StageBase.hpp>

namespace ez::rpl {

template <typename R, typename... Stages>
auto run(R&& range, Stages&&... stages)
{
    //for (auto&& val : range) { stage::invoke(val, stages...); }
}

}  // namespace ez::rpl
