#pragma once

#include <ez/async/detail/WhenAny.hpp>

namespace ez::async {

template <trait::Awaitable... Awaitables>
auto when_any(Awaitables&&... awaitables)
{
    using TasksTuple =
        Tuple<detail::WhenAnyContinuationTask<typename trait::AwaitableTraits<Awaitables>::R>...>;

    using Awaiter = detail::WhenAnyAwaiter<TasksTuple>;

    return Awaiter{detail::make_when_any_continuation_task(EZ_FWD(awaitables))...};
}

}  // namespace ez::async
