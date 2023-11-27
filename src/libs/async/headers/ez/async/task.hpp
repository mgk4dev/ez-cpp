#pragma once

#include <ez/async/receiver.hpp>
#include <ez/async/types.hpp>

namespace ez::async {

enum class AwaitReturnMode { ConstRef, Move };

template <typename T>
struct TaskPromise;

////////////////////////////////////////////////////////////////////////////////

template <typename T, async::AwaitReturnMode return_mode>
struct StoreCallerAwaiter {
    using Promise = TaskPromise<T>;

    Coroutine<Promise> corountine;

    bool await_ready() noexcept { return corountine.promise().has_value(); }

    Coroutine<> await_suspend(Coroutine<> caller) noexcept
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
    Coroutine<> await_suspend(auto current) noexcept { return current.promise().continuation; }
    void await_resume() noexcept {}
};

////////////////////////////////////////////////////////////////////////////////

template <typename T>
struct TaskPromise : public Receiver<T> {
    Coroutine<> continuation = std::noop_coroutine();

    void set_continuation(Coroutine<> cont) { continuation = cont; }
    auto get_return_object() noexcept { return make_coroutine(*this); }

    std::suspend_always initial_suspend() noexcept { return {}; }
    TaskFinalAwaiter final_suspend() noexcept { return {}; }
};

template <typename T = void>
class [[nodiscard]] Task {
public:
    using Promise = async::TaskPromise<T>;
    using promise_type = Promise;

    Task(Coroutine<Promise> handle) noexcept : m_coroutine{handle} {}
    Task(const Task&) = delete;
    Task& operator=(const Task& another) = delete;

    Task(Task&& another) { std::swap(m_coroutine, another.m_coroutine); }
    Task& operator=(Task&& rhs)
    {
        std::swap(m_coroutine, rhs.m_coroutine);
        return *this;
    }

    ~Task()
    {
        if (m_coroutine) {
            m_coroutine.destroy();
        }
    }

    async::StoreCallerAwaiter<T, async::AwaitReturnMode::ConstRef> operator co_await()
        const& noexcept
    {
        return {m_coroutine};
    }

    async::StoreCallerAwaiter<T, async::AwaitReturnMode::Move> operator co_await() && noexcept
    {
        return {m_coroutine};
    }

    bool done() const { return  m_coroutine.done(); }
    void resume() { m_coroutine.resume(); }
    void* address() const { return m_coroutine.address(); }

private:
    Coroutine<Promise> m_coroutine = nullptr;
};
}  // namespace ez::async
