#pragma once

#include <ez/rpl/Pipeline.hpp>
#include <ez/rpl/StageFactory.hpp>

namespace ez::rpl {

template <typename InputType, typename... StageFactories>
struct Compose {
    static constexpr size_t stage_count = sizeof...(StageFactories);

    using FirstStageFactory = EZ_TYPE_AT(meta::type_list<StageFactories...>, 0);
    using LastStageFactory = EZ_TYPE_AT(meta::type_list<StageFactories...>, stage_count - 1);

    static constexpr ProcessingMode input_processing_mode =
        FirstStageFactory::input_processing_mode;
    static constexpr ProcessingMode output_processing_mode =
        LastStageFactory::output_processing_mode;

    using PipelineType = Pipeline<input_processing_mode, InputType, StageFactories...>;
    using OutputType = PipelineType::OutputType;
    using InputTypeList = PipelineType::InputTypeList;

    PipelineType pipeline;

    Compose(auto&&... factories) : pipeline{std::in_place, EZ_FWD(factories)...} {}

    void process_incremental(InputType val, auto&& )
    {
        pipeline.first().process_incremental(static_cast<InputType>(val));
    }

    void process_batch(InputType val, auto&& next)
    {
        next.process_bach(pipeline.first().process_batch(static_cast<InputType>(val)));
    }

    decltype(auto) flush_to(auto&& next) { return next.process_batch(pipeline.last().flush()); }
};

template <typename... StageFactories>
auto compose(StageFactories&&... factories)
{
    static constexpr size_t stage_count = sizeof...(StageFactories);

    static_assert(stage_count > 0, "Compose requires at least one stage");

    if constexpr (stage_count > 0) {
        using FirstStageFactory = EZ_TYPE_AT(meta::type_list<StageFactories...>, 0);
        using LastStageFactory = EZ_TYPE_AT(meta::type_list<StageFactories...>, stage_count - 1);

        static constexpr ProcessingMode input_processing_mode =
            std::remove_cvref_t<FirstStageFactory>::input_processing_mode;
        static constexpr ProcessingMode output_processing_mode =
            std::remove_cvref_t<LastStageFactory>::output_processing_mode;

        return make_factory<input_processing_mode, output_processing_mode, Compose,
                            StageFactories...>(std::forward<StageFactories>(factories)...);
    }
}

}  // namespace ez::rpl
