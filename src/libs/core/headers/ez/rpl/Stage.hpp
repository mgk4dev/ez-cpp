#pragma once

#include <ez/rpl/StageBase.hpp>

#include <ez/TypeUtils.hpp>

namespace ez::rpl {

template <size_t Id, typename Sequence, typename InputType, typename StageFactory>
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

    Sequence& sequence() { return static_cast<Sequence&>(*this); }
    const Sequence& sequence() const { return static_cast<const Sequence&>(*this); }

    decltype(auto) get(Index<Id>) { return *this; }
    decltype(auto) get(Index<Id>) const { return *this; }

    decltype(auto) next() { return sequence().stage_at(Index<Id + 1>{}); }

    constexpr bool done() const
    {
        if constexpr (requires { m_stage.done(); }) { return m_stage.done(); }
        else {
            return false;
        }
    }

    constexpr bool any_done() const { return sequence().any_done(Index<Id>{}); }

    constexpr decltype(auto) end()
    {
        static_assert(input_processing_mode == ProcessingMode::Incremental,
                      "End called on non incremental processing stage.");
        if constexpr (requires { m_stage.end(next()); }) {
            static_assert(!std::is_void_v<decltype(m_stage.end(next()))>,
                          "End must not return void.");
            return m_stage.end(next());
        }
        else {
            return next().end();
        }
    }

    // Only accept InputType with exactly matching reference type.
    template <typename T>
    void process_incremental(T&& t) = delete;
    void process_incremental(InputType t)
    {
        constexpr bool has_process_incremental =
            requires { m_stage.process_incremental(static_cast<InputType>(t), next()); };

        static_assert(input_processing_mode == ProcessingMode::Incremental,
                      "process_incremental called on stage that is not incremental.");
        static_assert(
            input_processing_mode == ProcessingMode::Incremental || has_process_incremental,
            "Stage with incremental processing mode does not implement "
            "process_incremental.");
        if constexpr (has_process_incremental) {
            m_stage.process_incremental(static_cast<InputType>(t), next());
        }
    }

    // Only accept InputType with exactly matching reference type.
    template <typename T,
              typename = std::enable_if_t<input_processing_mode == ProcessingMode::Batch>>
    decltype(auto) process_batch(T&&) = delete;

    decltype(auto) process_batch(InputType t)
    {
        constexpr bool has_process_batch =
            requires { m_stage.process_batch(static_cast<InputType>(t), next()); };

        static_assert(has_process_batch,
                      "Stage with batch processing mode does not implement process_batch.");
        static_assert(
            !std::is_void_v<decltype(m_stage.process_batch(static_cast<InputType>(t), next()))>,
            "process_batch must not return void.");

        return m_stage.process_batch(static_cast<InputType>(t), next());
    }

    template <typename Container,
              typename = void,
              typename = std::enable_if_t<input_processing_mode == ProcessingMode::Incremental>>
    decltype(auto) process_batch(Container&& container)
    {
        constexpr bool has_process_incremental =
            requires { m_stage.process_incremental(std::declval<InputType>(), next()); };

        static_assert(
            has_process_incremental,
            "Stage with incremental processing mode does not implement process_incremental.");

        for (InputType input : std::forward<Container>(container)) {
            if (any_done()) break;
            m_stage.process_incremental(static_cast<InputType>(input), next());
        }
        return end();
    }
};

}  // namespace ez::rpl
