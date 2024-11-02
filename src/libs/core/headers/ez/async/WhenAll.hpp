#pragma once

#include <ez/async/internal/WhenAll.hpp>

namespace ez::async {
template <trait::Awaitable... Awaitables>
auto when_all(Awaitables&&... awaitables)
{
    using TasksTuple = Tuple<internal::WhenAllContinuationTask<
        typename trait::AwaitableTraits<std::decay_t<Awaitables>>::R>...>;

    using Awaiter = internal::WhenAllAwaiter<TasksTuple>;

    return Awaiter{internal::make_when_all_continuation_task(EZ_FWD(awaitables))...};
}

}  // namespace ez::async
