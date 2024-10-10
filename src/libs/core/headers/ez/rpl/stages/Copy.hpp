#pragma once

#include <ez/rpl/StageFactory.hpp>

#include <ez/Utils.hpp>

#include <vector>

namespace ez::rpl {

template <typename InputType>
struct ToVector {
    using Vector = std::vector<std::remove_cvref_t<InputType>>;
    using OutputType = Vector&&;

    Vector result;

    void process_incremental(InputType input, auto&&)
    {
        result.push_back(static_cast<InputType>(input));
    }

    decltype(auto) flush_to(auto&& next) { return next.process_batch(std::move(result)); }
};

inline auto to_vector()
{
    return make_factory<ProcessingMode::Incremental, ProcessingMode::Batch, ToVector>();
}

}  // namespace ez::rpl
