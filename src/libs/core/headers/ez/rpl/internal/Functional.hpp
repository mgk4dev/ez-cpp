#pragma once

#include <ez/Tuple.hpp>

#include <functional>

namespace ez::rpl::internal {

template <typename T>
struct IsTupleLike : std::false_type {
};

template <typename... Ts>
struct IsTupleLike<std::tuple<Ts...>> : std::true_type {
};

template <typename... Ts>
struct IsTupleLike<Tuple<Ts...>> : std::true_type {
};

template <typename T1, typename T2>
struct IsTupleLike<std::pair<T1, T2>> : std::true_type {
};

template <typename T>
consteval bool is_tuple_like()
{
    return IsTupleLike<T>::value;
}

template <typename F, typename TupleLike, size_t... indices>
constexpr bool can_flatten_args(IndexSequence<indices...>)
{
    return requires { std::declval<F>()(tuple::get<indices>(std::declval<TupleLike>())...); };
}

template <typename F, typename TupleLike>
constexpr bool can_flatten_args()
{
    return can_flatten_args<F, TupleLike>(
        std::make_index_sequence<tuple::tuple_size_v<std::remove_cvref_t<TupleLike>>>());
}

template <typename F, typename Arg>
decltype(auto) invoke_f_tuple(F&& f, Arg&& arg) requires(can_flatten_args<F, Arg>())
{
    return tuple::apply(EZ_FWD(f), EZ_FWD(arg));
}

template <typename F, typename Arg>
decltype(auto) invoke_f_tuple(F&& f, Arg&& arg) requires(!can_flatten_args<F, Arg>())
{
    return std::invoke(EZ_FWD(f), EZ_FWD(arg));
}

template <typename F, typename Arg>
decltype(auto) apply_fn(F&& f, Arg&& arg) requires(is_tuple_like<std::remove_cvref_t<Arg>>())
{
    return invoke_f_tuple(EZ_FWD(f), EZ_FWD(arg));
}

template <typename F, typename Arg>
decltype(auto) apply_fn(F&& f, Arg&& arg) requires(!is_tuple_like<std::remove_cvref_t<Arg>>())
{
    return std::invoke(EZ_FWD(f), EZ_FWD(arg));
}

}  // namespace ez::rpl::internal
