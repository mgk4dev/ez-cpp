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

}  // namespace ez
