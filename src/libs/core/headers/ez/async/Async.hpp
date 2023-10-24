#pragma once

#include <ez/Resource.hpp>
#include <ez/async/Receiver.hpp>

#include <coroutine>
#include <stdexcept>

namespace ez {
template <typename P = void>
using Coroutine = std::coroutine_handle<P>;
}  // namespace ez

namespace ez::async {

///////////////////////////////////////////////////////////////////////////////

class BrokenPromise : public std::runtime_error {
public:
    BrokenPromise() : std::runtime_error{"Broken promise"} {}
};

///////////////////////////////////////////////////////////////////////////////

enum class AwaitReturnMode { ConstRef, Move };

////////////////////////////////////////////////////////////////////////////////

template <typename Self>
struct ReturnValue {
    template <typename U>
    void return_value(U&& val)
    {
        static_cast<Self&>(*this).set_value(std::forward<U>(val));
    }

    template <typename U>
    std::suspend_always yield_value(U&& val)
    {
        static_cast<Self&>(*this).set_value(std::forward<U>(val));
        return {};
    }
};

template <typename Self>
struct ReturnVoid {
    void return_void() { static_cast<Self&>(*this).set_value(); }
};

////////////////////////////////////////////////////////////////////////////////

template <typename T>
struct PromiseBase;

template <typename T>
using PromiseReturn = std::
    conditional_t<std::is_same_v<T, void>, ReturnVoid<PromiseBase<T>>, ReturnValue<PromiseBase<T>>>;

template <typename T>
struct PromiseBase : public Receiver<T>, public PromiseReturn<T> {
    Coroutine<> caller = std::noop_coroutine();
    void set_caller(Coroutine<> cont) { caller = cont; }
};

struct CoroutineDeleter {
    void operator()(auto coroutine) const
    {
        if (coroutine) { coroutine.destroy(); }
    }
};

template <typename T,
          typename InitialSuspend,
          typename FinalSuspend,
          template <typename, typename, AwaitReturnMode>
          typename Awaiter>
struct Promise : public PromiseBase<T> {
    class Async {
    public:
        using promise_type = Promise;

        Async(Coroutine<Promise> handle) noexcept : m_coroutine{handle} {}

        Async(Async&& another) : m_coroutine{std::move(another.m_coroutine)} {}

        Awaiter<T, promise_type, AwaitReturnMode::ConstRef> operator co_await() const& noexcept
        {
            return {m_coroutine.get()};
        }

        Awaiter<T, promise_type, AwaitReturnMode::Move> operator co_await() && noexcept
        {
            return {m_coroutine.get()};
        }

        bool is_ready() const { return !m_coroutine.get() || m_coroutine.get().done(); }

        void resume() { m_coroutine.get().resume(); }

    private:
        Resource<Coroutine<promise_type>, CoroutineDeleter> m_coroutine;
    };

    Async get_return_object() noexcept { return Async{Coroutine<Promise>::from_promise(*this)}; }

    InitialSuspend initial_suspend() noexcept { return {}; }
    FinalSuspend final_suspend() noexcept { return {}; }
};

////////////////////////////////////////////////////////////////////////////////

template <typename T, typename Promise, async::AwaitReturnMode return_mode>
struct StoreCallerAwaiter {
    Coroutine<Promise> corountine;

    bool await_ready() noexcept { return corountine.promise().has_value(); }

    Coroutine<> await_suspend(Coroutine<> caller) noexcept
    {
        corountine.promise().set_caller(caller);
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

struct ResumeToCallerFinalSuspend {
    bool await_ready() noexcept { return false; }

    Coroutine<> await_suspend(auto current) noexcept { return current.promise().caller; }

    void await_resume() noexcept {}
};

////////////////////////////////////////////////////////////////////////////////

// clang-format off
template <
    typename T,
    typename InitialSuspend,
    typename FinalSuspend = ResumeToCallerFinalSuspend,
    template <typename , typename , AwaitReturnMode> typename Awaiter = StoreCallerAwaiter>
// clang-format on
using Async = Promise<T, InitialSuspend, FinalSuspend, Awaiter>::Async;

}  // namespace ez::async
