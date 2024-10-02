#pragma once

#include <ez/rpl/StageBase.hpp>

#include <ez/Utils.hpp>

namespace ez::rpl {

template <typename InputType, typename Predicate>
struct Filter : StageBase {
    using OutputType = InputType;

    Predicate predicate;

    Filter(auto&& p) : predicate{EZ_FWD(p)} {}

    void process_incremental(InputType val, auto&& next)
    {
        if (predicate(std::as_const(val))) { next.process_incremental(val); }
    }

    void process_complete(InputType range, auto&& next)
    {
        for(auto&& val : range)

        if (predicate(std::as_const(val))) { next.process_incremental(val); }
    }
};

template <typename Predicate>
auto filter(Predicate&& predicate)
{
    return stage::make<Filter>(std::forward<Predicate>(predicate));
}

}  // namespace ez::rpl
