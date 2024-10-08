#pragma once

#include <ez/rpl/Chain.hpp>
#include <ez/rpl/StageFactory.hpp>

namespace ez::rpl {

template <typename InputType, typename... StageFactories>
struct Compose {
    // using FirstStageFactory = EZ_TYPE_AT(meta::type_list<StageFactories...>, 0);
    // using LastStageFactory = EZ_TYPE_AT(meta::type_list<StageFactories...>,
    //                                     sizeof...(StageFactories) - 1);

    // using FirstStageImpl = decltype(std::declval<FirstStageFactory>().make(meta::type<InputType>));

    // static constexpr ProcessingMode input_processing_mode = FirstStageImpl::input_processing_mode;
    // static constexpr ProcessingMode output_processing_mode = ProcessingMode::Incremental;

    // using ChainType = Chain<    //                         FirstStageImpl::input_processing_mode,
    //                         InputType,
    //                         StageFactories...>;

    // using ChainInputTypeList = ChainType::InputTypeList;

    // static constexpr auto last_input_type =
    //     ChainInputTypeList{}.at(Index<sizeof...(StageFactories) - 1>{});

    // using OutputType = ChainType::OutputType;

    // using LastStageImpl = decltype(std::declval<LastStageFactory>().make(last_input_type));

    using ChainType = Chain<ProcessingMode::Incremental, InputType, StageFactories...>;

    ChainType chain;

    Compose(auto&&... factories): chain{in_place, EZ_FWD(factories)...} {}

    // template <typename Next,
    //           typename = std::enable_if_t<input_processing_mode == ProcessingMode::Incremental>>
    // void process_incremental(InputType val, Next&& next)
    // {
    //     next.process_incremental(chain.begin().process_incremental(static_cast<InputType>(val)));
    // }

    // template <typename Next,
    //           typename = std::enable_if_t<input_processing_mode == ProcessingMode::Batch>>
    // void process_batch(InputType val, Next&& next)
    // {
    //     next.process_bach(chain.begin().process_batch(static_cast<InputType>(val)));
    // }
};

template <typename... StageFactories>
auto compose(StageFactories&&... factories)
{
    static_assert(sizeof...(factories) > 0, "Compose requires at least one stage");

    if constexpr (sizeof...(factories) > 0) {
        return make_factory<Compose, StageFactories...>(std::forward<StageFactories>(factories)...);
    }
}

}  // namespace ez::rpl
