#pragma once

#include <ez/rpl/StageFactory.hpp>

#include <ez/Utils.hpp>

namespace ez::rpl {

template <typename InputType, typename Predicate>
struct Filter {
    using OutputType = InputType;
    EZ_RPL_STAGE_INFO(ProcessingStyle::Incremental, ProcessingStyle::Incremental)

    Predicate predicate;

    Filter(auto&& p) : predicate{EZ_FWD(p)} {}

    void process_incremental(InputType val, auto&& next)
    {
        if (predicate(std::as_const(val))) { next.process_incremental(val); }
    }
};

template <typename Predicate>
auto filter(Predicate predicate)
{
    return make_factory<Filter, Predicate>(std::move(predicate));
}

}  // namespace ez::rpl
