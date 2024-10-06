#pragma once

#include <ez/rpl/StageBase.hpp>

#include <ez/Utils.hpp>

#include <functional>

namespace ez::rpl {

template <typename F>
struct ForEach {
    F function;
};

template <typename F>
auto for_each(F&& f)
{
}

}  // namespace ez::rpl
