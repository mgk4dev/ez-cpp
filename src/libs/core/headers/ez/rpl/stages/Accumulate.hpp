#pragma once

#include <ez/rpl/StageFactory.hpp>

#include <ez/Option.hpp>

namespace ez::rpl {

template <typename InputType, typename Init, typename BinaryOp>
struct Accumulate {
    using OutputType = Init&&;

    Init init;
    BinaryOp binary_op;

    void process_incremental(InputType input, auto&&)
    {
        init = binary_op(std::move(init), static_cast<InputType>(input));
    }

    decltype(auto) flush_to(auto&& next) { return next.process_batch(std::move(init)); }
};

template <typename Init, typename BinaryOp = std::plus<>>
inline auto accumulate(Init&& init = {}, BinaryOp&& binary_op = {})
{
    return make_factory<ProcessingMode::Incremental, ProcessingMode::Batch, Accumulate, Init,
                        BinaryOp>(std::forward<Init>(init), std::forward<BinaryOp>(binary_op));
}

}  // namespace ez::rpl
