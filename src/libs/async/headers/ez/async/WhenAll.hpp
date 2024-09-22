#pragma once

#include <ez/async/detail/WhenAll.hpp>

namespace ez::async {

template <trait::Awaitable... Awaitables>
auto when_all(Awaitables&&... awaitables)
{
    using TasksTuple = Tuple<detail::WhenAllContinuationTask<
        typename trait::AwaitableTraits<std::decay_t<Awaitables>>::R>...>;

    using Awaiter = detail::WhenAllAwaiter<TasksTuple>;

    return Awaiter{detail::make_when_all_continuation_task(EZ_FWD(awaitables))...};
}

}  // namespace ez::async
