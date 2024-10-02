#pragma once

#include <type_traits>
#include <utility>

namespace ez::rpl {

enum class StageKind { Complete, Incremental };

namespace stage {

void invoke(auto&&) {}

void invoke(auto&& value, auto&& stage, auto&&... stages) { stage.process(value, stages...); }

template <template <typename...> typename Stage, typename T>
auto make(T&& arg)
{
    return 0;
    // return Stage<std::remove_cv_t<T>>{std::forward<T>(arg)};
}

}  // namespace stage

struct StageBase {
};

}  // namespace ez::rpl
