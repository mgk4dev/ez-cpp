#pragma once

#include <ez/Enum.hpp>
#include <ez/Utils.hpp>

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
struct PromiseStorage {
    using Storage = std::conditional_t<std::is_same_v<T, void>, Unit, T>;
    Enum<std::monostate, Storage, std::exception_ptr> result = std::monostate{};

    template <typename... U>
    void set_value(U&&... val)
    {
        result = Storage{std::forward<U>(val)...};
    }

    bool has_value() const noexcept { return !result.template is<std::monostate>(); }

    decltype(auto) get() const&
    {
        if (result.template is<std::exception_ptr>())
            std::rethrow_exception(result.template as<std::exception_ptr>());

        if constexpr (std::is_same_v<T, void>) { return; }
        else {
            return result.template as<T>();
        }
    }

    decltype(auto) get() &
    {
        if (result.template is<std::exception_ptr>())
            std::rethrow_exception(result.template as<std::exception_ptr>());

        if constexpr (std::is_same_v<T, void>) { return; }
        else {
            return result.template as<T>();
        }
    }

    void unhandled_exception() { result = std::current_exception(); }
};

template <typename T>
struct PromiseBase;

template <typename T>
using PromiseReturn = std::
    conditional_t<std::is_same_v<T, void>, ReturnVoid<PromiseBase<T>>, ReturnValue<PromiseBase<T>>>;

template <typename T>
struct PromiseBase : public PromiseStorage<T>, public PromiseReturn<T> {
    Coroutine<> caller = std::noop_coroutine();
    void set_caller(Coroutine<> cont) { caller = cont; }
};

template <typename T, typename AsyncType, typename InitialSuspend, typename FinalSuspend>
struct Promise : public PromiseBase<T> {
    AsyncType get_return_object() noexcept
    {
        return AsyncType{Coroutine<Promise>::from_promise(*this)};
    }

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

    Coroutine<> await_suspend(auto current) noexcept
    {
        return current.promise().caller;
    }

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
class Async {
public:
    using promise_type = Promise<T, Async, InitialSuspend, FinalSuspend>;

    explicit Async(Coroutine<promise_type> handle) noexcept : m_coroutine(handle) {}

    Async(const Async&) = delete;
    Async(Async&& another) : m_coroutine{std::exchange(another.m_coroutine, {})} {}
    ~Async()
    {
        if (m_coroutine) { m_coroutine.destroy(); }
    }

    Async& operator=(const Async&) = delete;
    Async& operator=(Async&& rhs)
    {
        std::swap(m_coroutine, rhs.m_coroutine);
        return *this;
    }

    Awaiter<T, promise_type, AwaitReturnMode::ConstRef> operator co_await() const& noexcept
    {
        return {m_coroutine};
    }

    Awaiter<T, promise_type, AwaitReturnMode::Move> operator co_await() && noexcept { return {m_coroutine}; }

    bool is_ready() const { return !m_coroutine || m_coroutine.done(); }

    void resume() { m_coroutine.resume(); }

private:
    Coroutine<promise_type> m_coroutine;
};



}  // namespace ez::async
