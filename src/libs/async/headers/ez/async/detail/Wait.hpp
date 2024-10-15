#pragma once

#include <ez/Resource.hpp>
#include <ez/async/Receiver.hpp>
#include <ez/async/Traits.hpp>
#include <ez/async/Types.hpp>

#include <condition_variable>
#include <mutex>

namespace ez::async::detail {
class SyncWaitEvent {
public:
    void set()
    {
        {
            std::lock_guard lock{m_mutex};
            m_set = true;
        }

        m_condition_var.notify_all();
    }

    void wait()
    {
        std::unique_lock lock{m_mutex};
        m_condition_var.wait(lock, [this] { return m_set; });
    }

private:
    std::mutex m_mutex;
    std::condition_variable m_condition_var;
    bool m_set = false;
};

///////////////////////////////////////////////////////////////////////////////

template <typename R>
class SyncWaitTask;

template <typename R>
class SyncWaitPromise : public Receiver<R> {
public:
    void start(SyncWaitEvent& event)
    {
        m_event = &event;
        make_coroutine(*this).resume();
    }

    auto get_return_object() noexcept { return make_coroutine(*this); }

    std::suspend_always initial_suspend() noexcept { return {}; }

    auto final_suspend() noexcept
    {
        struct CompletionNotifier {
            auto await_ready() const noexcept { return false; }
            auto await_suspend(CoHandle<SyncWaitPromise<R>> coroutine) const noexcept
            {
                coroutine.promise().m_event->set();
            }
            auto await_resume() noexcept {};
        };

        return CompletionNotifier{};
    }

private:
    SyncWaitEvent* m_event = nullptr;
};

///////////////////////////////////////////////////////////////////////////////

template <typename R>
class SyncWaitTask {
public:
    using Promise = SyncWaitPromise<R>;
    using promise_type = Promise;

    SyncWaitTask(CoHandle<Promise> coroutine) : m_coroutine{coroutine} {}

    SyncWaitTask(SyncWaitTask&& another) : m_coroutine{std::move(another.m_coroutine)} {}

    void start(SyncWaitEvent& event) noexcept { m_coroutine->promise().start(event); }

    decltype(auto) get()
    {
        if constexpr (std::is_same_v<void, R>) { m_coroutine->promise().get(); }
        else {
            return m_coroutine->promise().get();
        }
    }

private:
    Resource<CoHandle<Promise>, CoroutineDeleter> m_coroutine;
};

///////////////////////////////////////////////////////////////////////////////

template <trait::Awaitable Awaitable>
auto make_sync_wait_task(Awaitable awaitable)
    -> SyncWaitTask<typename trait::AwaitableTraits<Awaitable>::R>
{
    using R = typename trait::AwaitableTraits<Awaitable>::R;

    if constexpr (std::is_void_v<R>) {
        co_await std::move(awaitable);
        co_return;
    }
    else {
        co_return co_await std::move(awaitable);
    }
}

}  // namespace ez::async::detail
