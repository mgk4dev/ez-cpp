#pragma once

#include <ez/rpl/StageFactory.hpp>

#include <ez/Utils.hpp>

namespace ez::rpl {

template <typename InputType, typename Predicate>
struct Filter {
    using OutputType = InputType;
    EZ_RPL_STAGE_INFO(ProcessingMode::Incremental, ProcessingMode::Incremental)

    Predicate predicate;

    Filter(auto&& p) : predicate{EZ_FWD(p)} {}

    void process_incremental(InputType val, auto&& next)
    {
        if (predicate(std::as_const(val))) {
            next.process_incremental(static_cast<InputType>(val));
        }
    }
};

template <typename P>
auto filter(P&& predicate)
{
    return make_factory<Filter, P>(std::forward<P>(predicate));
}

}  // namespace ez::rpl
