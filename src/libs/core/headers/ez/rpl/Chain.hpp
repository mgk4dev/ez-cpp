#pragma once

#include <ez/rpl/StageBase.hpp>

namespace ez::rpl {

template <typename Chain, size_t index, typename InputParamType, typename Stage>
struct ChainElement {
    using InputType = InputParamType;
    using OutputType = typename Stage::InputType;

    Stage stage;

    decltype(auto) get(std::integral_constant<size_t, index>) { return *this; }
    Chain& as_chain() { return static_cast<Chain&>(*this); }
    decltype(auto) next() { return as_chain().get(std::integral_constant<size_t, index + 1>{}); }

    decltype(auto) end()
    {
        if constexpr (requires { stage.end(next()); }) { return stage.end(next()); }
        else {
            return next().done();
        }
    }
    bool done() const
    {
        if constexpr (requires { stage.done(); }) { return stage.done(); }
        else {
            return false;
        }
    }

    void process_incremental(InputType val, auto&& next)
    {
        return stage.process_incremental(static_cast<InputType>(val), next);
    }

    void process_complete(InputType val)
    {
        if constexpr (Stage::kind == StageKind::Complete) {
            return stage.process_complete(static_cast<InputType>(val), next());
        }
        else {
            for (auto&& elem : val) {
                stage.process_incremental(static_cast<InputType>(val), next());
            }
            return end();
        }
    }
};


template <size_t ... indices, typename ... Stages>
struct Chaing :


}  // namespace ez::rpl
