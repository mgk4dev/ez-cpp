#pragma once

#include <ez/rpl/StageBase.hpp>

#include <ez/TypeUtils.hpp>

namespace ez::rpl {

template <typename InputType, typename StageFactory>
class Stage {
public:
    using StageImpl = StageFactory::template Stage<InputType>;
    using OutputType = typename StageImpl::OutputType;

    EZ_RPL_STAGE_INFO(StageImpl::input_processing_mode, StageImpl::input_processing_mode)

private:
    StageImpl m_stage;

public:
    static_assert(std::is_reference_v<InputType>, "InputType must be a reference.");
    static_assert(std::is_lvalue_reference_v<OutputType> || std::is_rvalue_reference_v<OutputType>,
                  "OutputType must be a reference.");

    explicit Stage(StageFactory& factory) : m_stage(factory.make(meta::type<InputType>)) {}

    constexpr decltype(auto) end(auto&& next)
    {
        static_assert(input_processing_mode == ProcessingMode::Incremental,
                      "End called on non incremental processing stage.");
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

    decltype(auto) process_batch(InputType t, auto&& next, auto&& any_done_fn)
    {
        if constexpr (input_processing_mode == ProcessingMode::Batch) {
            constexpr bool has_process_batch =
                requires { m_stage.process_batch(static_cast<InputType>(t), next); };

            static_assert(has_process_batch,
                          "Stage with batch processing mode does not implement process_batch.");
            if constexpr (has_process_batch) {
                static_assert(
                    !std::is_void_v<decltype(m_stage.process_batch(static_cast<InputType>(t), next))>,
                    "process_batch must not return void.");
                return m_stage.process_batch(static_cast<InputType>(t), next);
            }
        }
        else {
            constexpr bool has_process_incremental =
                requires { m_stage.process_incremental(static_cast<InputType>(t), next); };

            static_assert(
                has_process_incremental,
                "Stage with incremental processing mode does not implement process_incremental.");

            for (InputType input : t) {
                if (any_done_fn()) break;

                if constexpr (has_process_incremental) {
                    m_stage.process_incremental(static_cast<InputType>(input), next);
                }
            }
            return end();
        }
    }
};

}  // namespace ez::rpl
