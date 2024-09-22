#pragma once

#include <concepts>
#include <type_traits>

#define EZ_FWD(arg) std::forward<decltype(arg)>(arg)
#define EZ_DECAY_T(arg) std::decay_t<decltype(arg)>

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

template <typename T>
using Ref = std::reference_wrapper<T>;

///////////////////////////////////////////////////////////////////////////////

template <typename... Ts>
struct DebugType;

///////////////////////////////////////////////////////////////////////////////

struct NonCopiable {
    NonCopiable() = default;
    NonCopiable(NonCopiable&&) = default;
    NonCopiable& operator=(NonCopiable&&) = default;

    NonCopiable(const NonCopiable&) = delete;
    NonCopiable& operator=(const NonCopiable&) = delete;
};

///////////////////////////////////////////////////////////////////////////////

template <typename T, typename U>
concept Constexpr = requires { typename T::IsCompileTime; } and std::convertible_to<T, U>;

template <auto value_>
struct CompileTime {
    using Type = decltype(value_);
    consteval operator Type() const noexcept { return value_; }
    static inline constexpr Type value = value_;

    using IsCompileTime = void;
};

template <auto value>
constexpr CompileTime<value> constexpr_;  // compile time

namespace arg {
template <typename T>
constexpr bool prefer_pass_by_value =
    sizeof(T) <= 2 * sizeof(void*) && std::is_trivially_copy_constructible_v<T>;

template <typename T>
    requires std::is_class_v<T> || std::is_union_v<T> || std::is_array_v<T> || std::is_function_v<T>
constexpr bool prefer_pass_by_value<T> = false;

template <typename T>
    requires(!std::is_void_v<T>)
using in = std::conditional_t<prefer_pass_by_value<T>, T, T const&>;

}  // namespace arg

}  // namespace ez

#define EZ_CONSTEXP(val) ez::constexpr_<val>

///////////////////////////////////////////////////////////////////////////////

namespace ez {

/// Compile time for loops.
/// Usage
/// @code
/// auto tuple = std::make_tuple(10, 10, 10);
/// for_constexpr<0, 3>([&](auto index) { std::get<index.value>(tuple) = index.value; });
/// @endcode
///
template <auto begin, auto end, typename F, typename... Args>
constexpr void for_constexpr(F&& f, Args&&... args)
{
    static_assert(begin <= end, "Invalid for_constexpr indices");
    if constexpr (begin < end) {
        f(EZ_CONSTEXP(begin), args...);
        for_constexpr<begin + 1, end>(f, args...);
    }
}

}  // namespace ez
