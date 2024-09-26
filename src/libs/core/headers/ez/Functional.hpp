#pragma once

#include <memory>
#include <type_traits>
#include <utility>

namespace ez {
template <typename T>
class FunctionView;

///
/// FunctionView is non owning reference on a callable.
/// It is a more efficient replacement of std::function when the
/// callable don't have to be copied.
/// Usage:
///   @code
///   auto get42 = []() { return 42; };
///   FunctionView<int()> fv(get42);
///   ASSERT_EQ(fv(), 42);
///   @endcode

template <typename R, typename... T>
class FunctionView<R(T...)> {
public:
    FunctionView() = default;
    FunctionView(std::nullptr_t);

    template <typename Callable>
    FunctionView(
        Callable&& callable,
        typename std::enable_if<!std::is_same<typename std::remove_reference<Callable>::type,
                                              FunctionView>::value>::type* = nullptr);

    R operator()(T... params) const;
    explicit operator bool() const;

private:
    typedef R (*CallbackType)(void* callable, T... params);
    CallbackType m_callback = nullptr;
    void* m_callable;

    template <typename Callable>
    static R generic_callback(void* callable, T... params)
    {
        return (*reinterpret_cast<Callable*>(callable))(std::forward<T>(params)...);
    }
};
////////////////////////////////////////////////////////////////////////////////////////////////

template <typename T>
class FreeFunction;

template <typename R, typename... Ts>
class FreeFunction<R(Ts...)> {
public:
    typedef R (*FuncPtr)(Ts...);

    FreeFunction() = default;

    FreeFunction(FuncPtr ptr) : m_callable{ptr} {}

    R operator()(Ts... params) const { return m_callable(params...); }
    explicit operator bool() const { return m_callable != nullptr; }

private:
    FuncPtr m_callable = nullptr;
};

////////////////////////////////////////////////////////////////////////////////////////////////

template <typename R, typename... T>
inline FunctionView<R(T...)>::FunctionView(std::nullptr_t)
{
}

template <typename R, typename... T>
template <typename Callable>
inline FunctionView<R(T...)>::FunctionView(
    Callable&& callable,
    typename std::enable_if<
        !std::is_same<typename std::remove_reference<Callable>::type, FunctionView>::value>::type*)
    : m_callback(generic_callback<typename std::remove_reference<Callable>::type>),
      m_callable(const_cast<void*>(reinterpret_cast<const void*>(std::addressof(callable))))
{
}

template <typename R, typename... T>
inline R FunctionView<R(T...)>::operator()(T... params) const
{
    return m_callback(m_callable, std::forward<T>(params)...);
}

template <typename R, typename... T>
inline FunctionView<R(T...)>::operator bool() const
{
    return m_callback;
}

////////////////////////////////////////////////////////////////////////////////////////////////

namespace internal {
template <typename... Args>
struct FnNonConstOverload {
    template <typename R, typename T>
    constexpr auto operator()(R (T::*ptr)(Args...)) const noexcept -> decltype(ptr)
    {
        return ptr;
    }
    template <typename R, typename T>
    static constexpr auto of(R (T::*ptr)(Args...)) noexcept -> decltype(ptr)
    {
        return ptr;
    }
};
template <typename... Args>
struct FnConstOverload {
    template <typename R, typename T>
    constexpr auto operator()(R (T::*ptr)(Args...) const) const noexcept -> decltype(ptr)
    {
        return ptr;
    }
    template <typename R, typename T>
    static constexpr auto of(R (T::*ptr)(Args...) const) noexcept -> decltype(ptr)
    {
        return ptr;
    }
};

template <typename... Args>
struct FnOverload : FnConstOverload<Args...>, FnNonConstOverload<Args...> {
    using FnConstOverload<Args...>::of;
    using FnConstOverload<Args...>::operator();
    using FnNonConstOverload<Args...>::of;
    using FnNonConstOverload<Args...>::operator();
    template <typename R>
    constexpr auto operator()(R (*ptr)(Args...)) const noexcept -> decltype(ptr)
    {
        return ptr;
    }
    template <typename R>
    static constexpr auto of(R (*ptr)(Args...)) noexcept -> decltype(ptr)
    {
        return ptr;
    }
};

}  // namespace internal

///
/// Returns a pointer to an overloaded function.
/// The template parameter is the list of the argument types of the function.
/// Example:
/// @code
/// auto f(){};
/// int f() {return 44;};
/// auto fn = fn_overload<int>(f);
/// @endcode
template <typename... Args>
constexpr internal::FnOverload<Args...> fn_overload = {};
template <typename... Args>
constexpr internal::FnConstOverload<Args...> fn_const_overload = {};
template <typename... Args>
constexpr internal::FnNonConstOverload<Args...> fn_non_const_overload = {};

}  // namespace ez
