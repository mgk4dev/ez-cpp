#pragma once

#include <ez/async/Operation.hpp>
#include <ez/async/Task.hpp>
#include <ez/async/WhenAny.hpp>

namespace ez::async {
namespace detail {
template <typename... Operations>
struct Race {
    using ReturnType = typename detail::WhenAnyReturn<Operations...>::Type;
    using TaskType = Task<ReturnType>;
};

}  // namespace detail

template <typename... Op>
auto race(Operation<Op>&... operations) -> detail::Race<Operation<Op>...>::TaskType
{
    auto result = co_await when_any(operations...);
    unused(operations.cancel()...);
    co_return result;
}

template <typename... Op>
auto race(Operation<Op>&&... operations) -> detail::Race<Operation<Op>...>::TaskType
{
    auto result = co_await when_any(operations...);
    unused(operations.cancel()...);
    co_return result;
}

}  // namespace ez::async
