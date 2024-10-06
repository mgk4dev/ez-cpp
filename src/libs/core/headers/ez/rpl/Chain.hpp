#pragma once

#include <ez/rpl/Stage.hpp>

#include <ez/Tuple.hpp>
#include <ez/TypeUtils.hpp>

namespace ez::rpl {

template <typename InputType>
struct ChainTerminator {
    template <typename T>
    T process_batch(T&& t)
    {
        return std::forward<T>(t);
    }
};

///////////////////////////////////////////////////////////////////////

template <typename InputType, ProcessingMode>
constexpr auto get_chain_input_types_impl()
{
    return meta::type_list<>;
}

template <typename InputType,
          ProcessingMode previous_stage_mode,
          typename StageFactory,
          typename... StageFactories>
constexpr auto get_chain_input_types_impl()
{
    using Stage = StageFactory::template Stage<InputType>;

    constexpr ProcessingMode stage_input_processing_mode = Stage::input_processing_mode;
    constexpr ProcessingMode stage_ouput_processing_mode = Stage::output_processing_mode;

    using OutputType = Stage::OutputType;

    if constexpr (previous_stage_mode == ProcessingMode::Batch &&
                  stage_input_processing_mode == ProcessingMode::Incremental) {
        using ValueType =
            decltype(*std::begin(std::declval<std::add_lvalue_reference_t<OutputType>>()));

        return meta::type_list<ValueType> +
               get_chain_input_types_impl<ValueType, stage_ouput_processing_mode,
                                          StageFactories...>();
    }
    else {
        return meta::type_list<OutputType> +
               get_chain_input_types_impl<OutputType, stage_ouput_processing_mode,
                                          StageFactories...>();
    }
}

template <typename InputType, typename... StageFactories>
constexpr auto get_chain_input_types()
{
    return meta::type_list<InputType> +
           get_chain_input_types_impl<InputType, ProcessingMode::Batch,
                                      std::remove_cv_t<StageFactories>...>();
}

///////////////////////////////////////////////////////////////////////

template <typename InputType, typename... StageFactories>
struct ChainTraits {
    static constexpr size_t stage_count = sizeof...(StageFactories);
    using StageFactoryTypeList = TypeList<StageFactories...>;
    using InputTypeList = decltype(get_chain_input_types<InputType, StageFactories...>());

    using __LastStageFactoryType = EZ_TYPE_AT(StageFactoryTypeList{}, stage_count - 1);
    using __LastStageInputType = EZ_TYPE_AT(InputTypeList{}, stage_count - 1);

    using OutputType =
        typename __LastStageFactoryType::template Stage<__LastStageInputType>::OutputType;
};

///////////////////////////////////////////////////////////////////////

template <typename InputType, typename... StageFactories>
struct Chain;

///////////////////////////////////////////////////////////////////////

template <size_t Id, typename ChainInputType, typename... ChainStageFactories>
class ChainElement {
public:
    using Traits = ChainTraits<ChainInputType, ChainStageFactories...>;

    using InputTypeList = Traits::InputTypeList;

    using InputType = EZ_TYPE_AT(InputTypeList{}, Id);

    using StageFactoryTypeList = Traits::StageFactoryTypeList;

    using Stage = EZ_TYPE_AT(StageFactoryTypeList{}, Id)::template Stage<InputType>;
    using OutputType = typename Stage::OutputType;

    static constexpr auto input_processing_mode = Stage::input_processing_mode;

private:
    Stage m_stage;

public:
    static_assert(std::is_reference_v<InputType>, "InputType must be a reference.");
    static_assert(std::is_lvalue_reference_v<OutputType> || std::is_rvalue_reference_v<OutputType>,
                  "OutputType must be a reference.");

    explicit ChainElement(Stage&& stage) : m_stage(std::move(stage)) {}

    constexpr decltype(auto) end(auto&& next)
    {
        static_assert(input_processing_mode == ProcessingMode::Incremental,
                      "End called on a batch processing "
                      "stage.");
        if constexpr (requires { m_stage.end(next); }) {
            static_assert(!std::is_void_v<decltype(m_stage.end(next))>,
                          "End must not return void.");
            return m_stage.end(next);
        }
        else {
            return next.end();
        }
    }

    // Only accept InputType with exactly matching reference type.
    template <typename T>
    void process_incremental(T&& t) = delete;
    void process_incremental(InputType t, auto&& next)
    {
        constexpr bool has_process_incremental =
            requires { m_stage.process_incremental(static_cast<InputType>(t), next); };

        static_assert(input_processing_mode == ProcessingMode::Incremental,
                      "process_incremental called on stage that is not incremental.");
        static_assert(
            input_processing_mode == ProcessingMode::Incremental || has_process_incremental,
            "Stage with incremental processing mode does not implement "
            "process_incremental.");
        if constexpr (has_process_incremental) {
            m_stage.process_incremental(static_cast<InputType>(t), next);
        }
    }

    // Only accept InputType with exactly matching reference type.
    template <typename T>
    decltype(auto) process_batch(T&&) = delete;

    decltype(auto) process_batch(InputType t, auto&& next, auto&& any_done)
    {
        if constexpr (input_processing_mode == ProcessingMode::Batch) {
            constexpr bool has_process_batch =
                requires { m_stage.process_batch(static_cast<InputType>(t), next); };

            static_assert(has_process_batch,
                          "Stage with batch processing mode does not implement "
                          "process_batch.");
            if constexpr (has_process_batch) {
                static_assert(!std::is_void_v<decltype(m_stage.process_batch(
                                  static_cast<InputType>(t), next))>,
                              "process_batch must not return void.");
                return m_stage.process_batch(static_cast<InputType>(t), next);
            }
        }
        else {
            constexpr bool has_process_incremental =
                requires { m_stage.process_incremental(static_cast<InputType>(t), next); };

            static_assert(has_process_incremental,
                          "Stage with incremental processing mode does not implement "
                          "process_incremental.");

            for (InputType input : t) {
                if (any_done()) break;

                if constexpr (has_process_incremental) {
                    m_stage.process_incremental(static_cast<InputType>(input), next);
                }
            }
            return end();
        }
    }
};

///////////////////////////////////////////////////////////////////////

template <typename...>
struct ChainElements;

template <typename InputType, typename... StageFactories, size_t... Ids>
struct ChainElements<Chain<InputType, StageFactories...>, IndexSequence<Ids...>> {
    using ChainType = Chain<InputType, StageFactories...>;

    using Traits = ChainTraits<InputType, StageFactories...>;

    using InputTypeList = Traits::InputTypeList;
    using OutputType = typename Traits::OutputType;

    template <size_t Index>
    using Element = ChainElement<Index, InputType, StageFactories...>;

    Tuple<StageFactories...> stage_factories;
    Tuple<Element<Ids>...> chain_elements;
    ChainTerminator<OutputType> terminator;

    ChainElements(auto&&... factories)
        : stage_factories{EZ_FWD(factories)...}, chain_elements{make_stage(Index<Ids>())...}
    {
    }

    auto& as_chain() { return static_cast<ChainType&>(*this); }

    template <size_t Id>
    decltype(auto) chain_element_at(Index<Id>)
    {
        if constexpr (Id < chain_elements.size())
            return std::get<Id>(this->chain_elements);
        else
            return terminator;
    }

    template <size_t Id>
    auto make_stage(Index<Id>)
    {
        using StageInputType = EZ_TYPE_AT(InputTypeList{}, Id);
        return std::get<Id>(stage_factories).make(meta::type<StageInputType>);
    }
};

template <typename InputType, typename... StageFactories>
struct Chain
    : ChainElements<Chain<InputType, StageFactories...>, IndexSequenceFor<StageFactories...>> {
    using ChainElementsType =
        ChainElements<Chain<InputType, StageFactories...>, IndexSequenceFor<StageFactories...>>;

    Chain(auto&&... factories) : ChainElementsType{EZ_FWD(factories)...} {}
};

}  // namespace ez::rpl
