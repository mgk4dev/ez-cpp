#pragma once

#include <ez/async/detail/when_all.hpp>

namespace ez::async {

template <trait::Awaitable... Awaitables>
auto when_all(Awaitables... awaitables)
{
    using TasksTuple =
        Tuple<detail::ContinuationTask<typename trait::AwaitableTraits<Awaitables>::R>...>;

    using Awaiter = detail::WhenAllAwaiter<TasksTuple>;

    return Awaiter{detail::make_continuation_task(std::move(awaitables))...};
}

}  // namespace ez::async
