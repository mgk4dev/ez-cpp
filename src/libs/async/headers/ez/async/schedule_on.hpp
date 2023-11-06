#pragma once

#include <ez/async/task.hpp>
#include <ez/async/executor.hpp>

namespace ez::async {

template <typename E>
struct ScheduleAwaiter {
    E* executor;

    constexpr bool await_ready() const noexcept { return false; }
    void await_suspend(Coroutine<> coroutine) const
    {
        ez::async::Executor<E>::post(*executor, [coroutine] { coroutine.resume(); });
    }
    constexpr void await_resume() const noexcept {}
};

template <typename Executor>
auto schedule_on(Executor& executor)
{
    return ScheduleAwaiter<Executor>{&executor};
}

}  // namespace ez::async
