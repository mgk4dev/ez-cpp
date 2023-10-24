#pragma once

#include <concepts>
#include <type_traits>

namespace ez {

struct Inplace {};
struct Unit {};

constexpr Inplace in_place{};

struct Noop {
    template <typename... Ts>
    void operator()(Ts&&...) const noexcept
    {
    }
};

constexpr Noop noop{};
constexpr Noop unused{};

///////////////////////////////////////////////////////////////////////////////

namespace arg {

namespace detail {
template <typename T>
constexpr bool pass_by_value =
    sizeof(T) <= 2 * sizeof(void*) && std::is_trivially_copy_constructible_v<T>;

template <typename T>
    requires std::is_class_v<T> || std::is_union_v<T> || std::is_array_v<T> || std::is_function_v<T>
constexpr bool pass_by_value<T> = false;
}  // namespace detail

template <typename T>
    requires(!std::is_void_v<T>)
using in = std::conditional_t<detail::pass_by_value<T>, T, T const&>;

template <typename T>
    requires(!std::is_void_v<T>)
using out = T&;

template <typename T>
    requires(!std::is_void_v<T>)
using inout = T&;

template <typename T>
    requires(!std::is_void_v<T>)
using move = T&&;

}  // namespace arg

///////////////////////////////////////////////////////////////////////////////

template <typename T, typename U>
concept Constexpr = requires { typename T::IsCompileTime; } and std::convertible_to<T, U>;

template <auto value>
struct CompileTime {
    using Type = decltype(value);
    consteval operator Type() const noexcept { return value; }
    using IsCompileTime = void;
};

template <auto value>
constexpr CompileTime<value> ct_; // compile time

}  // namespace ez
