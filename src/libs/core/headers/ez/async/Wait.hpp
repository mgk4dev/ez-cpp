#pragma once

#include <ez/async/internal/Wait.hpp>

#include <ez/Utils.hpp>

namespace ez::async {
auto sync_wait(trait::Awaitable auto awaitable)
{
    internal::SyncWaitEvent event;
    auto task = internal::make_sync_wait_task(std::move(awaitable));
    task.start(event);
    event.wait();
    return std::move(task.get());
}

}  // namespace ez::async
