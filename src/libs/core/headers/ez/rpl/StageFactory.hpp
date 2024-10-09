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

    template <typename T>
    static constexpr auto input_processing_mode = Stage<T>::input_processing_mode;

    template <typename T>
    static constexpr auto output_processing_mode = Stage<T>::output_processing_mode;

    explicit StageFactory(Inplace, auto&&... args) : params_tuple(EZ_FWD(args)...) {}

    StageFactory(const StageFactory&) = default;
    StageFactory(StageFactory&&) = default;

    template <typename InputType>
    auto make(Type<InputType>) &
    {
        return std::apply([](auto&&... args) { return Stage<InputType>{EZ_FWD(args)...}; },
                          params_tuple);
    }

    template <typename InputType>
    auto make(Type<InputType>) &&
    {
        return std::apply([](auto&&... args) { return Stage<InputType>{EZ_FWD(args)...}; },
                          std::move(params_tuple));
    }
};

template <template <typename...> typename StageTemplate, typename... ParameterTypes>
auto make_factory(auto&&... args)
{
    return StageFactory<StageTemplate, std::remove_cvref_t<ParameterTypes>...>(in_place,
                                                                               EZ_FWD(args)...);
}

}  // namespace ez::rpl
