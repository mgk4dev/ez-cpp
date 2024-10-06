#pragma once

#include <ez/Tuple.hpp>
#include <ez/TypeUtils.hpp>
#include <ez/rpl/StageBase.hpp>

namespace ez::rpl {

template <template <typename...> typename StageTemplate, typename... StageParameters>
struct StageFactory {
    template <typename T>
    using Stage = StageTemplate<T, StageParameters...>;

    std::tuple<StageParameters...> params_tuple;

    explicit StageFactory(auto&&... args) : params_tuple(EZ_FWD(args)...) {}

    StageFactory(const StageFactory&) = default;
    StageFactory(StageFactory&&) = default;

    template <typename InputType>
    auto make(Type<InputType>) const
    {
        return std::apply([](auto&&... args) { return Stage<InputType>{EZ_FWD(args)...}; },
                          std::move(params_tuple));
    }
};

template <template <typename...> typename StageTemplate, typename... ParameterTypes>
auto make_factory(auto&&... ts)
{
    return StageFactory<StageTemplate, std::remove_cv_t<ParameterTypes>...>(EZ_FWD(ts)...);
}

}  // namespace ez::rpl
