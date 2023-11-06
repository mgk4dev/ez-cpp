#pragma once

#include <ez/async/receiver.hpp>
#include <ez/async/trait.hpp>
#include <ez/async/types.hpp>
#include <ez/resource.hpp>
#include <ez/tuple.hpp>
#include <ez/utils.hpp>

#include <atomic>

namespace ez::async::detail {

class WhenAllLatch : NonCopiable {
public:
    WhenAllLatch(uint32_t count) : m_count(count + 1) {}

    WhenAllLatch(WhenAllLatch&& other)
        : m_count(other.m_count.load(std::memory_order::acquire)),
          m_awaiting(std::exchange(other.m_awaiting, nullptr))
    {
    }

    WhenAllLatch& operator=(WhenAllLatch&& other)
    {
        if (std::addressof(other) != this) {
            m_count.store(other.m_count.load(std::memory_order::acquire),
                          std::memory_order::relaxed);
            m_awaiting = std::exchange(other.m_awaiting, nullptr);
        }

        return *this;
    }

    bool is_ready() const noexcept { return m_awaiting && m_awaiting.done(); }

    bool try_await(Coroutine<> awaiting_coroutine) noexcept
    {
        m_awaiting = awaiting_coroutine;
        return m_count.fetch_sub(1, std::memory_order::acq_rel) > 1;
    }

    void notify_awaitable_completed() noexcept
    {
        if (m_count.fetch_sub(1, std::memory_order::acq_rel) == 1) { m_awaiting.resume(); }
    }

private:
    std::atomic_uint32_t m_count;
    Coroutine<> m_awaiting;
};

///////////////////////////////////////////////////////////////////////////////

template <typename Container>
class WhenAllAwaiter;

template <typename R>
class ContinuationTask;

///////////////////////////////////////////////////////////////////////////////

template <>
class WhenAllAwaiter<Tuple<>> {
public:
    constexpr WhenAllAwaiter() noexcept {}
    constexpr WhenAllAwaiter(Tuple<>) noexcept {}

    constexpr bool await_ready() const noexcept { return true; }
    void await_suspend(Coroutine<>) noexcept {}
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

    bool await_suspend(Coroutine<> awaiting_coroutine) noexcept
    {
        return try_await(awaiting_coroutine);
    }

    auto await_resume() & noexcept
    {
        return m_tasks.transformed([](auto&& task) { return EZ_FWD(task).get(); });
    }

    auto await_resume() && noexcept
    {
        return std::move(m_tasks).transformed([](auto&& task) { return EZ_FWD(task).get(); });
    }

private:
    bool try_await(Coroutine<> awaiting_coroutine)
    {
        m_tasks.for_each([&](auto& task) { task.start(m_latch); });
        return m_latch.try_await(awaiting_coroutine);
    }

private:
    Tuple<Tasks...> m_tasks;
    WhenAllLatch m_latch;
};

///////////////////////////////////////////////////////////////////////////////

template <typename R>
class ContinuationTask;

template <typename R>
class ContinuationPromise : public Receiver<R> {
public:
    using Self = ContinuationPromise<R>;

    auto get_return_object() noexcept { return make_coroutine(*this); }

    std::suspend_always initial_suspend() const noexcept { return {}; }

    auto final_suspend() const noexcept
    {
        struct CompletionNotifier {
            bool await_ready() const noexcept { return false; }
            void await_suspend(Coroutine<Self> coroutine) noexcept
            {
                coroutine.promise().m_latch->notify_awaitable_completed();
            }
            void await_resume() const noexcept {}
        };

        return CompletionNotifier{};
    }

    auto return_value(auto&& value)
    {
        Receiver<R>::set_value(EZ_FWD(value));
        return final_suspend();
    }

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
class ContinuationTask {
public:
    using Promise = ContinuationPromise<R>;
    using promise_type = Promise;

    ContinuationTask(Coroutine<Promise> coroutine) : m_coroutine{coroutine} {}
    ContinuationTask(ContinuationTask&& another) : m_coroutine{std::move(another.m_coroutine)} {}

    auto start(WhenAllLatch& latch) noexcept -> void { m_coroutine.get().promise().start(latch); }

    decltype(auto) get() & { return m_coroutine.get().promise().get(); }
    decltype(auto) get() && { return std::move(m_coroutine.get().promise()).get(); }

private:
    Resource<Coroutine<Promise>, CoroutineDeleter> m_coroutine;
};

///////////////////////////////////////////////////////////////////////////////

auto make_continuation_task(trait::Awaitable auto awaitable)
    -> ContinuationTask<typename trait::AwaitableTraits<decltype(awaitable)>::R>
{
    using R = typename trait::AwaitableTraits<decltype(awaitable)>::R;
    if constexpr (std::is_void_v<R>) {
        co_await std::move(awaitable);
        co_return;
    }
    else {
        co_return co_await std::move(awaitable);
    }
}

}  // namespace ez::async::detail
