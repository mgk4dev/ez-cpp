#pragma once

#include <ez/async/Receiver.hpp>
#include <ez/async/Traits.hpp>
#include <ez/async/Types.hpp>

#include <ez/Resource.hpp>
#include <ez/Shared.hpp>
#include <ez/Tuple.hpp>
#include <ez/TypeUtils.hpp>
#include <ez/Utils.hpp>

#include <atomic>

namespace ez::async::detail {
class WhenAnyLatch {
public:
    WhenAnyLatch() {}
    WhenAnyLatch(const WhenAnyLatch&) = default;

    WhenAnyLatch(WhenAnyLatch&& other)
    {
        std::swap(m_finished_count, other.m_finished_count);
        std::swap(m_continuation, other.m_continuation);
    }

    WhenAnyLatch& operator=(WhenAnyLatch&& other)
    {
        std::swap(m_finished_count, other.m_finished_count);
        std::swap(m_continuation, other.m_continuation);
        return *this;
    }

    WhenAnyLatch& operator=(const WhenAnyLatch& other) = default;

    bool is_ready() const noexcept { return m_finished_count->load() > 0; }

    void set_continuation(CoHandle<> awaiting_coroutine) noexcept
    {
        m_continuation = awaiting_coroutine;
    }

    void notify_awaitable_completed() noexcept
    {
        auto old_count = m_finished_count->fetch_add(1, std::memory_order::acquire);
        if (old_count == 0) { m_continuation.resume(); }
    }

private:
    Shared<std::atomic_uint32_t> m_finished_count{in_place, 0};
    CoHandle<> m_continuation;
};

///////////////////////////////////////////////////////////////////////////////

template <typename Container>
class WhenAnyAwaiter;

template <typename R>
class WhenAnyContinuationTask;

///////////////////////////////////////////////////////////////////////////////

template <>
class WhenAnyAwaiter<Tuple<>> {
public:
    constexpr WhenAnyAwaiter() noexcept {}
    constexpr WhenAnyAwaiter(Tuple<>) noexcept {}

    constexpr bool await_ready() const noexcept { return true; }
    void await_suspend(CoHandle<>) noexcept {}
    Tuple<> await_resume() const noexcept { return {}; }
};

///////////////////////////////////////////////////////////////////////////////

template <typename... Ts>
struct WhenAnyReturn {
    using ReturnTypes = TypeList<typename Ts::ReturnType...>;
    using UniqueTypes = decltype(meta::remove_duplicates(ReturnTypes{}));
    using FrontType = typename decltype(meta::front_type(UniqueTypes{}))::Inner;
    using Type = std::conditional_t<UniqueTypes{}.count == 1,
                                    FrontType,
                                    typename UniqueTypes::template ApplyTo<Enum>>;
};

template <typename... Tasks>
class WhenAnyAwaiter<Tuple<Tasks...>> {
public:
    using ReturnType = typename WhenAnyReturn<Tasks...>::Type;

    WhenAnyAwaiter(Tasks... tasks) : m_tasks{std::move(tasks)...}, m_latch{} {}

    WhenAnyAwaiter(WhenAnyAwaiter&& other)
        : m_tasks{std::move(other.m_tasks)}, m_latch{std::move(other.m_latch)}
    {
    }

    bool await_ready() const noexcept { return m_latch.is_ready(); }

    bool await_suspend(CoHandle<> awaiting_coroutine) noexcept
    {
        return start_tasks(awaiting_coroutine);
    }

    auto await_resume() &
    {
        if constexpr (std::is_same_v<ReturnType, void>) {
            m_tasks.for_each([&](auto& task) {
                if (task.done()) task.get();
            });
        }
        else {
            ReturnType result{};
            m_tasks.for_each([&](auto& task) {
                if (task.done()) result = task.get();
            });
            return result;
        }
    }

    auto await_resume() &&
    {
        if constexpr (std::is_same_v<ReturnType, void>) {
            tuple::for_each(m_tasks, [&](auto& task) {
                if (task.done()) task.get();
            });
        }
        else {
            ReturnType result{};
            tuple::for_each(m_tasks, [&](auto& task) {
                if (task.done()) result = std::move(task.get());
            });
            return result;
        }
    }

private:
    bool start_tasks(CoHandle<> awaiting_coroutine)
    {
        m_latch.set_continuation(awaiting_coroutine);
        tuple::for_each(m_tasks, [&](auto& task) { task.start(m_latch); });
        return !m_latch.is_ready();
    }

private:
    Tuple<Tasks...> m_tasks;
    WhenAnyLatch m_latch;
};

///////////////////////////////////////////////////////////////////////////////

template <typename R>
class WhenAnyContinuationTask;

template <typename R>
class WhenAnyContinuationPromise : public Receiver<R> {
public:
    using Self = WhenAnyContinuationPromise<R>;

    auto get_return_object() noexcept { return make_coroutine(*this); }

    std::suspend_always initial_suspend() const noexcept { return {}; }

    auto final_suspend() const noexcept
    {
        struct CompletionNotifier {
            bool await_ready() const noexcept { return false; }
            void await_suspend(CoHandle<Self> coroutine) noexcept
            {
                coroutine.promise().m_latch.notify_awaitable_completed();
            }
            void await_resume() const noexcept {}
        };

        return CompletionNotifier{};
    }

    void return_value(auto&& value) { Receiver<R>::set_value(EZ_FWD(value)); }

    void start(WhenAnyLatch latch)
    {
        m_latch = latch;
        make_coroutine(*this).resume();
    }

private:
    WhenAnyLatch m_latch;
};

///////////////////////////////////////////////////////////////////////////////

template <typename R>
class WhenAnyContinuationTask {
public:
    using Promise = WhenAnyContinuationPromise<R>;
    using promise_type = Promise;
    using ReturnType = R;

    WhenAnyContinuationTask(CoHandle<Promise> coroutine) : m_coroutine{coroutine} {}
    WhenAnyContinuationTask(WhenAnyContinuationTask&& another)
        : m_coroutine{std::move(another.m_coroutine)}
    {
    }

    auto start(WhenAnyLatch& latch) noexcept -> void { m_coroutine.get().promise().start(latch); }

    decltype(auto) get() & { return m_coroutine.get().promise().get(); }
    decltype(auto) get() && { return std::move(m_coroutine.get().promise()).get(); }

    bool done() const { return m_coroutine.get().done(); }

    void* address() const { return m_coroutine.get().address(); }

private:
    Resource<CoHandle<Promise>, CoroutineDeleter> m_coroutine;
};

///////////////////////////////////////////////////////////////////////////////

template <trait::Awaitable T>
auto make_when_any_continuation_task(T&& awaitable)
    -> WhenAnyContinuationTask<typename trait::AwaitableTraits<decltype(awaitable)>::R>
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

}  // namespace ez::async::detail
