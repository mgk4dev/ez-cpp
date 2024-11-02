#pragma once

#include <ez/async/Operation.hpp>
#include <ez/async/Task.hpp>
#include <ez/async/WhenAny.hpp>

#include <ez/Traits.hpp>

namespace ez::async {

namespace internal {

template <typename... Operations>
struct Race {
    using ReturnType = typename WhenAnyReturn<Operations...>::Type;
    using TaskType = Task<ReturnType>;
};

}  // namespace internal

auto race(trait::IsTemplate<Operation> auto&&... operations)
    -> internal::Race<EZ_REMOVE_CVR_T(operations)...>::TaskType
{
    auto result = co_await when_any(operations...);
    unused(operations.cancel()...);
    co_return result;
}

}  // namespace ez::async
