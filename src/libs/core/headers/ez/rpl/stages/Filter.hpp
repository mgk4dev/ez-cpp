#pragma once

#include <ez/rpl/StageFactory.hpp>

#include <ez/Option.hpp>

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

template <typename InputType, typename Equals>
struct SkipDuplicates {
    using OutputType = InputType;

    Equals equals;

    std::conditional_t<std::is_lvalue_reference_v<InputType>,
                       std::remove_reference_t<InputType>*,
                       Option<std::remove_cvref_t<InputType>>>
        reference;

    void process_incremental(InputType input, auto&& next)
    {
        const bool has_value(reference);
        const bool is_equal = has_value && equals(input, *reference);
        if (!is_equal) {
            if (has_value) { next.process_incremental(*std::move(reference)); }
            if constexpr (std::is_lvalue_reference_v<InputType>) { reference = &input; }
            else {
                reference.emplace(static_cast<InputType>(input));
            }
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

template <typename Equals = std::equal_to<>>
auto skip_duplicates(Equals&& equals = {})
{
    return make_factory<ProcessingMode::Incremental, ProcessingMode::Incremental, SkipDuplicates,
                        Equals>(std::forward<Equals>(equals));
}

}  // namespace ez::rpl
