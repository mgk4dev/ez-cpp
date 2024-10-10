#pragma once

#include <ez/rpl/StageFactory.hpp>

#include <ez/Utils.hpp>

namespace ez::rpl {

template <typename InputType, typename Predicate>
struct Filter {
    using OutputType = InputType;

    Predicate predicate;

    Filter(auto&& p) : predicate{EZ_FWD(p)} {}

    void process_incremental(InputType val, auto&& next)
    {
        if (predicate(std::as_const(val))) {
            next.process_incremental(static_cast<InputType>(val));
        }
    }
};

struct DefaultFilterPredicate {
    constexpr bool operator()(auto&& val) { return bool(std::as_const(val)); }
};

template <typename Predicate = DefaultFilterPredicate>
auto filter(Predicate&& predicate = {})
{
    return make_factory<ProcessingMode::Incremental, ProcessingMode::Incremental, Filter,
                        Predicate>(std::forward<Predicate>(predicate));
}

}  // namespace ez::rpl
