#pragma once

#include <ez/rpl/StageBase.hpp>

#include <ez/Tuple.hpp>
#include <ez/TypeUtils.hpp>

namespace ez::rpl {

template <typename InputType>
struct ChainTerminator {
    template <typename T>
    T process_all(T&& t)
    {
        return std::forward<T>(t);
    }
};

///////////////////////////////////////////////////////////////////////

template <typename... Ts>
struct ChainTraits;

template <typename InputType>
struct ChainTraits<InputType> {
    using InputTypeList = TypeList<InputType>;
};

template <typename InputType, typename StageFactory, typename... StageFactories>
struct ChainTraits<InputType, StageFactory, StageFactories...> {
    static constexpr size_t stage_count = sizeof...(StageFactories) + 1;
    using StageFactoryTypeList = TypeList<StageFactory, StageFactories...>;

    using __LastStageFactoryType =
        decltype(StageFactoryTypeList::at(Index<stage_count - 1>{}))::Inner;

    using __StageOutputType = typename StageFactory::template Stage<InputType>::OutputType;
    using __RestTypeList =
        typename ChainTraits<__StageOutputType, StageFactories...>::InputTypeList;
    using InputTypeList = decltype(meta::type_list<InputType> + __RestTypeList{});
    using __LastStageInputType = decltype(InputTypeList::at(Index<stage_count - 1>()))::Inner;

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

    static constexpr auto input_type = InputTypeList::at(Index<Id>{});
    using InputType = decltype(input_type)::Inner;

    using StageFactoryTypeList = Traits::StageFactoryTypeList;

    static constexpr auto factory_type = StageFactoryTypeList::at(Index<Id>{});
    using Stage = decltype(factory_type)::Inner::template Stage<InputType>;
    using OutputType = typename Stage::OutputType;

    EZ_RPL_STAGE_INFO(Stage::input_processing_style, Stage::output_processing_style)

private:
    Stage m_stage;

public:
    // static_assert(std::is_reference_v<InputType>, "InputType must be a reference.");
    // static_assert(std::is_lvalue_reference_v<OutputType> ||
    // std::is_rvalue_reference_v<OutputType>,
    //               "OutputType must be a reference.");

    // decltype(auto) get(Index<Id>) { return *this; }
    // decltype(auto) get(Index<Id>) const { return *this; }

    explicit ChainElement(Stage&& stage) : m_stage(std::move(stage)) {}

    // auto& as_chain() { return static_cast<Chain&>(*this); }

    // auto& as_chain() const { return static_cast<const Chain&>(*this); }

    // decltype(auto) next() { return as_chain().chain_element_at(Index<Id + 1>()); }

    // constexpr decltype(auto) end()
    // {
    //     static_assert(input_processing_style == ProcessingStyle::Incremental,
    //                   "End called on a complete processing "
    //                   "stage.");
    //     if constexpr (requires { m_stage.end(next()); }) {
    //         static_assert(!std::is_void_v<decltype(m_stage.end(next()))>,
    //                       "End must not return void.");
    //         return m_stage.end(next());
    //     }
    //     else {
    //         return next().end();
    //     }
    // }
    // constexpr bool done() const
    // {
    //     if constexpr (requires { m_stage.done(); }) { return m_stage.done(); }
    //     else {
    //         return false;
    //     }
    // }

    // constexpr bool any_done() const { return as_chain().any_done(Index<Id>()); }

    // // Only accept InputType with exactly matching reference type.
    // template <typename T>
    // void process_incremental(T&& t) = delete;
    // void process_incremental(InputType t)
    // {
    //     constexpr bool has_process_incremental = requires
    //     {
    //         m_stage.process_incremental(static_cast<InputType>(t), next());
    //     };

    //     static_assert(input_processing_style == ProcessingStyle::Incremental,
    //                   "process_incremental called on stage that is not incremental.");
    //     static_assert(
    //         input_processing_style == ProcessingStyle::Incremental || has_process_incremental,
    //         "Stage with incremental processing style does not implement "
    //         "process_incremental.");
    //     if constexpr (has_process_incremental) {
    //         m_stage.process_incremental(static_cast<InputType>(t), next());
    //     }
    // }

    // // Only accept InputType with exactly matching reference type.
    // template <typename T>
    // decltype(auto) process_all(T&&) = delete;

    // decltype(auto) process_all(InputType t)
    // {
    //     if constexpr (input_processing_style == ProcessingStyle::All) {
    //         constexpr bool has_process_all = requires
    //         {
    //             m_stage.process_all(static_cast<InputType>(t), next());
    //         };

    //         static_assert(has_process_all,
    //                       "Stage with complete processing style does not implement "
    //                       "process_all.");
    //         if constexpr (has_process_all) {
    //             static_assert(!std::is_void_v<decltype(
    //                               m_stage.process_all(static_cast<InputType>(t), next()))>,
    //                           "process_all must not return void.");
    //             return m_stage.process_all(static_cast<InputType>(t), next());
    //         }
    //     }
    //     else {
    //         constexpr bool has_process_incremental = requires
    //         {
    //             m_stage.process_incremental(static_cast<InputType>(t), next());
    //         };

    //         static_assert(has_process_incremental,
    //                       "Stage with incremental processing style does not implement "
    //                       "process_incremental.");

    //         for (InputType input : t) {
    //             if (any_done()) break;

    //             if constexpr (has_process_incremental) {
    //                 m_stage.process_incremental(static_cast<InputType>(input), next());
    //             }
    //         }
    //         return end();
    //     }
    // }
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
    auto make_stage(Index<Id> index)
    {
        using InputType = decltype(InputTypeList::at(index))::Inner;
        return std::get<Id>(stage_factories).make(meta::type<InputType>);
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
