#pragma once

#include <ez/Tuple.hpp>
#include <ez/TypeUtils.hpp>
#include <ez/rpl/StageBase.hpp>

namespace ez::rpl {

template <ProcessingMode in_processing_mode,
          ProcessingMode out_processing_mode,
          template <typename...>
          typename StageTemplate,
          typename... StageParameters>
struct StageFactory {
    template <typename T>
    using Stage = StageTemplate<T, StageParameters...>;

    static constexpr ProcessingMode input_processing_mode = in_processing_mode;
    static constexpr ProcessingMode output_processing_mode = out_processing_mode;

    std::tuple<StageParameters...> params_tuple;

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

template <ProcessingMode in_processing_mode,
          ProcessingMode out_processing_mode,
          template <typename...>
          typename StageTemplate,
          typename... ParameterTypes>
auto make_factory(ParameterTypes&&... args)
{
    return StageFactory<in_processing_mode, out_processing_mode, StageTemplate,
                        std::remove_cvref_t<ParameterTypes>...>(in_place, EZ_FWD(args)...);
}

}  // namespace ez::rpl
