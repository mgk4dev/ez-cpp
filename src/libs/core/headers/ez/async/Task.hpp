#pragma once

#include <ez/async/Receiver.hpp>
#include <ez/async/Types.hpp>

namespace ez::async {
enum class AwaitReturnMode { ConstRef, Move };

template <typename T>
struct TaskPromise;

////////////////////////////////////////////////////////////////////////////////

template <typename T, async::AwaitReturnMode return_mode>
struct StoreCallerAwaiter {
    using Promise = TaskPromise<T>;

    CoHandle<Promise> corountine;

    bool await_ready() noexcept { return corountine.promise().has_value(); }

    CoHandle<> await_suspend(CoHandle<> caller) noexcept
    {
        corountine.promise().set_continuation(caller);
        return corountine;
    }

    decltype(auto) await_resume()
    {
        if (!corountine) throw async::BrokenPromise{};

        if constexpr (std::is_same_v<T, void> ||
                      (return_mode == async::AwaitReturnMode::ConstRef)) {
            return corountine.promise().get();
        }
        else {
            return std::move(corountine.promise().get());
        }
    }
};

////////////////////////////////////////////////////////////////////////////////

struct TaskFinalAwaiter {
    bool await_ready() noexcept { return false; }
    CoHandle<> await_suspend(auto current) noexcept { return current.promise().continuation; }
    void await_resume() noexcept {}
};

////////////////////////////////////////////////////////////////////////////////

template <typename T>
struct TaskPromise : public Receiver<T> {
    CoHandle<> continuation = std::noop_coroutine();

    void set_continuation(CoHandle<> cont) { continuation = cont; }
    auto get_return_object() noexcept { return make_coroutine(*this); }

    std::suspend_always initial_suspend() noexcept { return {}; }
    TaskFinalAwaiter final_suspend() noexcept { return {}; }
};

template <typename T = void>
class [[nodiscard]] Task {
public:
    using Promise = async::TaskPromise<T>;
    using promise_type = Promise;

    Task(CoHandle<Promise> handle) noexcept : m_coroutine{handle} {}
    Task(const Task&) = delete;
    Task& operator=(const Task& another) = delete;

    Task(Task&& another) = default;
    Task& operator=(Task&& rhs) = default;

    async::StoreCallerAwaiter<T, async::AwaitReturnMode::ConstRef> operator co_await()
        const& noexcept
    {
        return {m_coroutine.get()};
    }

    async::StoreCallerAwaiter<T, async::AwaitReturnMode::Move> operator co_await() && noexcept
    {
        return {m_coroutine.get()};
    }

    bool done() const { return handle().done(); }
    void resume() { handle().resume(); }
    void* address() const { return handle().address(); }
    CoHandle<> handle() const { return m_coroutine.get(); }

private:
    UniqueCoroutine<Promise> m_coroutine{nullptr};
};
}  // namespace ez::async
