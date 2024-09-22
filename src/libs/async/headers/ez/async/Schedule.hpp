#pragma once

#include <ez/async/Executor.hpp>
#include <ez/async/Task.hpp>

namespace ez::async {

template <typename E>
struct ScheduleAwaiter {
    E* executor;

    constexpr bool await_ready() const noexcept { return false; }
    void await_suspend(Coroutine<> coroutine) const
    {
        async::post(*executor, [coroutine] { coroutine.resume(); });
    }
    constexpr void await_resume() const noexcept {}
};

template <typename Executor>
auto schedule_on(Executor& executor)
{
    return ScheduleAwaiter<Executor>{&executor};
}

}  // namespace ez::async
