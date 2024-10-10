#pragma once

#include <ez/rpl/Chain.hpp>
#include <ez/rpl/StageFactory.hpp>

namespace ez::rpl {

template <typename InputType, typename... StageFactories>
struct Parallel {
    static constexpr size_t stage_count = sizeof...(StageFactories);

    static constexpr bool all_inputs_batch =
        ((StageFactories::input_processing_mode == ProcessingMode::Batch) && ...);

    static constexpr bool all_inputs_incremental =
        ((StageFactories::input_processing_mode == ProcessingMode::Incremental) && ...);

    static constexpr bool all_outputs_batch =
        ((StageFactories::output_processing_mode == ProcessingMode::Batch) && ...);

    static constexpr bool all_outputs_incremental =
        ((StageFactories::output_processing_mode == ProcessingMode::Incremental) && ...);

    static_assert(all_inputs_batch || all_inputs_incremental,
                  "All stages must have the same input processing mode");

    using FirstStageFactory = EZ_TYPE_AT(meta::type_list<StageFactories...>, 0);
    using LastStageFactory = EZ_TYPE_AT(meta::type_list<StageFactories...>, stage_count - 1);

    static constexpr ProcessingMode input_processing_mode =
        FirstStageFactory::input_processing_mode;
    static constexpr ProcessingMode output_processing_mode =
        FirstStageFactory::output_processing_mode;

    template <typename Factory>
    using ChainType = Chain<input_processing_mode, InputType, Factory>;

    using ChainTuple = Tuple<ChainType<StageFactories>...>;
    using OutputType = Tuple<typename ChainType<StageFactories>::OutputType...>&&;

    ChainTuple chains;

    Parallel(auto&&... factories)
        : chains{ChainType<StageFactories>{in_place, EZ_FWD(factories)}...}
    {
    }

    void process_incremental(InputType val, auto&&)
    {
        tuple::for_each(chains, [&](auto& chain) {
            chain.first().process_incremental(static_cast<InputType>(val));
        });
    }

    template <size_t... indices>
    void process_batch_impl(InputType val, auto&& next, IndexSequence<indices...>)
    {
        next.process_bach(Tuple{
            chains[EZ_CONSTEXP(indices)].first().process_batch(static_cast<InputType>(val))...});
    }

    void process_batch(InputType val, auto&& next)
    {
        process_batch_impl(static_cast<InputType>(val), EZ_FWD(next),
                           IndexSequenceFor<StageFactories...>{});
    }

    template <size_t... indices>
    decltype(auto) flush_to_impl(auto&& next, IndexSequence<indices...>)
    {
        return next.process_batch(Tuple{chains[EZ_CONSTEXP(indices)].first().flush()...});
    }

    decltype(auto) flush_to(auto&& next)
    {
        return flush_to_impl(EZ_FWD(next), IndexSequenceFor<StageFactories...>{});
    }
};

template <typename... StageFactories>
auto parallel(StageFactories&&... factories)
{
    static constexpr size_t stage_count = sizeof...(StageFactories);

    static_assert(stage_count > 0, "Parallel requires at least one stage");

    if constexpr (stage_count > 0) {
        using FirstStageFactory = EZ_TYPE_AT(meta::type_list<StageFactories...>, 0);
        using LastStageFactory = EZ_TYPE_AT(meta::type_list<StageFactories...>, stage_count - 1);

        static constexpr ProcessingMode input_processing_mode =
            std::remove_cvref_t<FirstStageFactory>::input_processing_mode;
        static constexpr ProcessingMode output_processing_mode =
            std::remove_cvref_t<LastStageFactory>::output_processing_mode;

        return make_factory<input_processing_mode, output_processing_mode, Parallel,
                            StageFactories...>(std::forward<StageFactories>(factories)...);
    }
}

}  // namespace ez::rpl
