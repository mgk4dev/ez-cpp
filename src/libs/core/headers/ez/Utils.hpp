#pragma once

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

inline constexpr Noop noop{};
inline constexpr Noop unused{};

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

}  // namespace ez
