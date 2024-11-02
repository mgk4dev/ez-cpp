#pragma once

#include <ez/async/internal/WhenAny.hpp>

namespace ez::async {
template <trait::Awaitable... Awaitables>
auto when_any(Awaitables&&... awaitables)
{
    using TasksTuple =
        Tuple<internal::WhenAnyContinuationTask<typename trait::AwaitableTraits<Awaitables>::R>...>;

    using Awaiter = internal::WhenAnyAwaiter<TasksTuple>;

    return Awaiter{internal::make_when_any_continuation_task(EZ_FWD(awaitables))...};
}

}  // namespace ez::async
