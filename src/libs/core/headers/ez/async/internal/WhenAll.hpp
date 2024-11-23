#pragma once

#include <ez/async/Receiver.hpp>
#include <ez/async/Traits.hpp>
#include <ez/async/Types.hpp>

#include <ez/Resource.hpp>
#include <ez/Tuple.hpp>
#include <ez/Utils.hpp>

#include <atomic>

namespace ez::async::internal {

class WhenAllLatch : NonCopiable {
public:
    WhenAllLatch(std::uint32_t count) : m_count(count + 1) {}

    WhenAllLatch(WhenAllLatch&& other) : m_count(other.m_count.load(std::memory_order::acquire))
    {
        std::swap(m_continuation, other.m_continuation);
    }

    WhenAllLatch& operator=(WhenAllLatch&& other)
    {
        if (std::addressof(other) != this) {
            m_count.store(other.m_count.load(std::memory_order::acquire),
                          std::memory_order::relaxed);
            std::swap(m_continuation, other.m_continuation);
        }

        return *this;
    }

    bool is_ready() const noexcept { return m_continuation && m_continuation.done(); }

    void set_continuation(CoHandle<> awaiting_coroutine) noexcept
    {
        m_continuation = awaiting_coroutine;
        m_count.fetch_sub(1, std::memory_order::acq_rel);
    }

    void notify_awaitable_completed() noexcept
    {
        if (m_count.fetch_sub(1, std::memory_order::acq_rel) == 1) { m_continuation.resume(); }
    }

private:
    std::atomic_uint32_t m_count;
    CoHandle<> m_continuation;
};

///////////////////////////////////////////////////////////////////////////////

template <typename Container>
class WhenAllAwaiter;

template <typename R>
class WhenAllContinuationTask;

///////////////////////////////////////////////////////////////////////////////

template <>
class WhenAllAwaiter<Tuple<>> {
public:
    constexpr WhenAllAwaiter() noexcept {}
    constexpr WhenAllAwaiter(Tuple<>) noexcept {}

    constexpr bool await_ready() const noexcept { return true; }
    void await_suspend(CoHandle<>) noexcept {}
    Tuple<> await_resume() const noexcept { return {}; }
};

///////////////////////////////////////////////////////////////////////////////

template <typename... Tasks>
class WhenAllAwaiter<Tuple<Tasks...>> {
public:
    WhenAllAwaiter(Tasks... tasks) : m_tasks{std::move(tasks)...}, m_latch{sizeof...(Tasks)} {}

    WhenAllAwaiter(WhenAllAwaiter&& other)
        : m_tasks{std::move(other.m_tasks)}, m_latch{std::move(other.m_latch)}
    {
    }

    bool await_ready() const noexcept { return m_latch.is_ready(); }

    bool await_suspend(CoHandle<> awaiting_coroutine) noexcept
    {
        return start_tasks(awaiting_coroutine);
    }

    auto await_resume() & noexcept
    {
        return tuple::transform(m_tasks, [](auto&& task) { return EZ_FWD(task).get(); });
    }

    auto await_resume() && noexcept
    {
        return tuple::transform(std::move(m_tasks), [](auto&& task) { return EZ_FWD(task).get(); });
    }

private:
    bool start_tasks(CoHandle<> awaiting_coroutine)
    {
        m_latch.set_continuation(awaiting_coroutine);
        m_tasks.for_each([&](auto& task) { task.start(m_latch); });
        return !m_latch.is_ready();
    }

private:
    Tuple<Tasks...> m_tasks;
    WhenAllLatch m_latch{0};
};

///////////////////////////////////////////////////////////////////////////////

template <typename R>
class WhenAllContinuationTask;

template <typename R>
class WhenAllContinuationPromise : public Receiver<R> {
public:
    using Self = WhenAllContinuationPromise<R>;

    auto get_return_object() noexcept { return make_coroutine(*this); }

    std::suspend_always initial_suspend() const noexcept { return {}; }

    auto final_suspend() const noexcept
    {
        struct CompletionNotifier {
            bool await_ready() const noexcept { return false; }
            void await_suspend(CoHandle<Self> coroutine) noexcept
            {
                coroutine.promise().m_latch->notify_awaitable_completed();
            }
            void await_resume() const noexcept {}
        };

        return CompletionNotifier{};
    }

    void return_value(auto&& value) { Receiver<R>::set_value(EZ_FWD(value)); }

    void start(WhenAllLatch& latch)
    {
        m_latch = &latch;
        make_coroutine(*this).resume();
    }

private:
    WhenAllLatch* m_latch = nullptr;
};

///////////////////////////////////////////////////////////////////////////////

template <typename R>
class WhenAllContinuationTask {
public:
    using Promise = WhenAllContinuationPromise<R>;
    using promise_type = Promise;

    WhenAllContinuationTask(CoHandle<Promise> coroutine) : m_coroutine{coroutine} {}
    WhenAllContinuationTask(WhenAllContinuationTask&& another)
        : m_coroutine{std::move(another.m_coroutine)}
    {
    }

    auto start(WhenAllLatch& latch) noexcept -> void { m_coroutine.get().promise().start(latch); }

    decltype(auto) get() & { return m_coroutine.get().promise().get(); }
    decltype(auto) get() && { return std::move(m_coroutine.get().promise()).get(); }

private:
    UniqueCoroutine<Promise> m_coroutine;
};

///////////////////////////////////////////////////////////////////////////////

template <trait::Awaitable T>
auto make_when_all_continuation_task(T&& awaitable)
    -> WhenAllContinuationTask<typename trait::AwaitableTraits<decltype(awaitable)>::R>
{
    using R = typename trait::AwaitableTraits<decltype(awaitable)>::R;
    if constexpr (std::is_void_v<R>) {
        co_await EZ_FWD(awaitable);
        co_return;
    }
    else {
        co_return co_await EZ_FWD(awaitable);
    }
}

}  // namespace ez::async::internal
