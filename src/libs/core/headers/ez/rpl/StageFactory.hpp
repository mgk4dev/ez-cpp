#pragma once

#include <ez/Tuple.hpp>
#include <ez/TypeUtils.hpp>
#include <ez/rpl/StageBase.hpp>

namespace ez::rpl {

template <typename ParamsTuple,
          template <typename...>
          typename StageTemplate,
          typename... StageParameters>
struct StageFactory {
    template <typename T>
    using Stage = StageTemplate<T, StageParameters...>;

    EZ_RPL_STAGE_INFO(Stage<int>::input_processing_style, Stage<int>::output_processing_style)

    ParamsTuple params_tuple;

    template <typename... Args>
    explicit StageFactory(Args&&... args) : params_tuple(std::forward<Args>(args)...)
    {
    }

    template <typename InputType>
    auto make(Type<InputType>) const
    {
        return std::apply([](auto&&... args) { return Stage<InputType>{EZ_FWD(args)...}; },
                          std::move(params_tuple));
    }
};

template <template <typename...> typename StageTemplate,
          typename... StageParameters,
          typename... Ts>
auto make_factory(Ts&&... ts)
{
    return StageFactory<std::tuple<std::decay_t<Ts>...>, StageTemplate, StageParameters...>(
        std::forward<Ts>(ts)...);
}

}  // namespace ez::rpl
