#pragma once

#include <ez/rpl/StageFactory.hpp>
#include <ez/rpl/internal/Functional.hpp>

#include <ez/Option.hpp>

namespace ez::rpl {

template <typename InputType, typename Predicate>
struct Filter {
    using OutputType = InputType;

    Predicate predicate;

    Filter(auto&& p) : predicate{EZ_FWD(p)} {}

    void process_incremental(InputType val, auto&& next)
    {
        if (internal::apply_fn(predicate, std::as_const(val))) {
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

    void set_reference(auto&& value)
    {
        if constexpr (std::is_lvalue_reference_v<InputType>) { reference = &value; }
        else {
            reference.emplace(value);
        }
    }

    void process_incremental(InputType input, auto&& next)
    {
        if (!reference || !equals(input, *reference)) {
            set_reference(input);
            next.process_incremental(static_cast<InputType>(input));
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
