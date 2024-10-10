#pragma once

#include <ez/rpl/Stage.hpp>

#include <ez/Tuple.hpp>
#include <ez/TypeUtils.hpp>

namespace ez::rpl {

struct ChainTerminator {
    template <typename T>
    T process_batch(T&& t)
    {
        return std::forward<T>(t);
    }
};

///////////////////////////////////////////////////////////////////////

template <ProcessingMode, typename InputType>
constexpr auto get_chain_input_types_impl()
{
    return meta::type_list<>;
}

template <ProcessingMode input_mode,
          typename InputType,
          typename StageFactory,
          typename... StageFactories>
constexpr auto get_chain_input_types_impl()
{
    constexpr ProcessingMode stage_input_processing_mode = StageFactory::input_processing_mode;
    constexpr ProcessingMode stage_ouput_processing_mode = StageFactory::output_processing_mode;

    if constexpr (input_mode == ProcessingMode::Batch &&
                  stage_input_processing_mode == ProcessingMode::Incremental) {
        using ValueType =
            decltype(*std::begin(std::declval<InputType>()));


        using T = std::conditional_t<std::is_reference_v<ValueType>,
                                     ValueType,
                                     std::add_rvalue_reference_t<ValueType>
            >;

        using Stage = StageFactory::template Stage<T>;
        using OutputType = Stage::OutputType;
        return meta::type_list<T> +
               get_chain_input_types_impl<stage_ouput_processing_mode, OutputType,
                                          StageFactories...>();
    }
    else {
        using Stage = StageFactory::template Stage<InputType>;
        using OutputType = Stage::OutputType;
        return meta::type_list<InputType> +
               get_chain_input_types_impl<stage_ouput_processing_mode, OutputType,
                                          StageFactories...>();
    }
}

template <ProcessingMode input_mode, typename InputType, typename... StageFactories>
constexpr auto get_chain_input_types()
{
    return get_chain_input_types_impl<input_mode, InputType,
                                      std::remove_cvref_t<StageFactories>...>();
}

///////////////////////////////////////////////////////////////////////

template <ProcessingMode input_mode, typename InputType, typename... StageFactories>
struct ChainTraits {
    static constexpr size_t stage_count = sizeof...(StageFactories);
    using StageFactoryTypeList = TypeList<StageFactories...>;
    using InputTypeList =
        decltype(get_chain_input_types<input_mode, InputType, StageFactories...>());

    using __LastStageFactoryType = EZ_TYPE_AT(StageFactoryTypeList{}, stage_count - 1);
    using __LastStageInputType = EZ_TYPE_AT(InputTypeList{}, stage_count - 1);

    using OutputType =
        typename __LastStageFactoryType::template Stage<__LastStageInputType>::OutputType;
};

///////////////////////////////////////////////////////////////////////

template <ProcessingMode input_mode, typename...>
struct ChainStages;

template <ProcessingMode input_mode,
          typename InputType,
          typename... StageFactories,
          size_t... indices>
struct ChainStages<input_mode, InputType, IndexSequence<indices...>, StageFactories...> {
    static constexpr size_t stage_count = sizeof...(StageFactories);

    using Traits = ChainTraits<input_mode, InputType, StageFactories...>;

    using InputTypeList = Traits::InputTypeList;
    using OutputType = typename Traits::OutputType;

    using FactoriesTypeList = TypeList<StageFactories...>;

    template <size_t index, typename Sequence>
    using StageT = Stage<index,
                         Sequence,
                         typename EZ_TYPE_AT(InputTypeList{}, index),
                         typename EZ_TYPE_AT(FactoriesTypeList{}, index)>;

    struct StageSequence : public StageT<indices, StageSequence>... {
        using StageT<indices, StageSequence>::StageT...;
        using StageT<indices, StageSequence>::get...;

        StageSequence(auto&&... factories)
            : StageT<indices, StageSequence>(
                  std::get<indices>(std::forward_as_tuple(EZ_FWD(factories)...)))...
        {
        }

        ChainTerminator terminator;

        template <size_t index>
        decltype(auto) stage_at(Index<index> id)
        {
            if constexpr (index < sizeof...(StageFactories))
                return this->get(id);
            else
                return terminator;
        }

        template <size_t index>
        constexpr bool any_done(Index<index>) const
        {
            return ((indices >= index && this->get(Index<indices>{}).done()) || ...);
        }
    };

    StageSequence stages;
    ChainStages(Inplace, auto&&... factories) : stages{EZ_FWD(factories)...} {}

    decltype(auto) first() { return stages.get(Index<0>{}); }
    decltype(auto) last() { return stages.get(Index<stage_count - 1>{}); }
};

template <ProcessingMode input_mode, typename InputType, typename... StageFactories>
struct Chain : ChainStages<input_mode,
                           InputType,
                           IndexSequenceFor<StageFactories...>,
                           std::remove_cvref_t<StageFactories>...> {
    using ChainStagesType = ChainStages<input_mode,
                                        InputType,
                                        IndexSequenceFor<StageFactories...>,
                                        std::remove_cvref_t<StageFactories>...>;

    using ChainStagesType::InputTypeList;
    using ChainStagesType::OutputType;

    Chain(Inplace, auto&&... factories) : ChainStagesType{in_place, EZ_FWD(factories)...} {}

    Chain(const Chain&) = default;
    Chain(Chain&&) = default;
};

template <typename InputType, typename... StageFactories>
auto make_chain(StageFactories&&... factories)
{
    return Chain<ProcessingMode::Batch, InputType, StageFactories...>{
        in_place, std::forward<StageFactories>(factories)...};
}

}  // namespace ez::rpl
