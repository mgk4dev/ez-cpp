#pragma once

#include <ez/Overload.hpp>
#include <ez/Utils.hpp>

#include <tuple>

namespace ez {
template <typename... Ts>
class Tuple;

namespace internal {
template <size_t... indices>
auto transform_impl(auto&& f, auto&& tuple, std::index_sequence<indices...>)
{
    return std::make_tuple(f(EZ_FWD(tuple)[EZ_CONSTEXP(indices)])...);
};

template <typename... Ts>
auto transform_tuple(const Tuple<Ts...>& tuple, auto&& f)
{
    return Tuple{transform_impl(f, tuple, std::make_index_sequence<sizeof...(Ts)>{})};
}

template <typename... Ts>
auto transform_tuple(Tuple<Ts...>& tuple, auto&& f)
{
    return Tuple{transform_impl(f, tuple, std::make_index_sequence<sizeof...(Ts)>{})};
}

template <typename... Ts>
auto transform_tuple(Tuple<Ts...>&& tuple, auto&& f)
{
    return Tuple{transform_impl(f, std::move(tuple), std::make_index_sequence<sizeof...(Ts)>{})};
}

}  // namespace internal

/// Extension of std::tuple with simpler access API.
/// Usage:
/// @code
/// Tuple tuple{1, 2, 3};
/// auto strings = tuple.transformed([](auto val) { return std::to_string(val); });
/// ASSERT_EQ(strings[constexpr_<0>], "1");
/// ASSERT_EQ(strings[constexpr_<1>], "2");
/// ASSERT_EQ(strings[constexpr_<2>], "3");
/// @endcode
template <typename... Ts>
class Tuple : public std::tuple<Ts...> {
public:
    using Super = std::tuple<Ts...>;
    using std::tuple<Ts...>::tuple;
    using std::tuple<Ts...>::operator=;

    Tuple(const std::tuple<Ts...>& t) : Super{t} {}
    Tuple(std::tuple<Ts...>& t) : Super{t} {}
    Tuple(std::tuple<Ts...>&& t) : Super{std::move(t)} {}

    consteval size_t size() const { return sizeof...(Ts); }

    constexpr decltype(auto) operator[](Constexpr<size_t> auto index) const&
    {
        return std::get<index.value>(*this);
    }

    constexpr decltype(auto) operator[](Constexpr<size_t> auto index) &&
    {
        return std::move(std::get<index.value>(*this));
    }

    constexpr decltype(auto) operator[](Constexpr<size_t> auto index) &
    {
        return std::get<index.value>(*this);
    }

    void for_each(auto&& f)
    {
        for_constexpr<0, sizeof...(Ts)>([&](auto index) { f(this->operator[](index)); });
    }

    void for_each(auto&& f) const
    {
        for_constexpr<0, sizeof...(Ts)>([&](auto index) { f(this->operator[](index)); });
    }

    auto transformed(auto&& f) & { return internal::transform_tuple(*this, EZ_FWD(f)); }
    auto transformed(auto&& f) const& { return internal::transform_tuple(*this, EZ_FWD(f)); }
    auto transformed(auto&& f) && { return internal::transform_tuple(std::move(*this), EZ_FWD(f)); }
};

template <class... Ts>
Tuple(Ts...) -> Tuple<Ts...>;

template <class... Ts>
Tuple(const std::tuple<Ts...>&) -> Tuple<Ts...>;

template <class... Ts>
Tuple(std::tuple<Ts...>&) -> Tuple<Ts...>;

template <class... Ts>
Tuple(std::tuple<Ts...>&&) -> Tuple<Ts...>;

namespace tuple {

template <typename T, typename F>
void for_each(T&& tuple, F&& f)
{
    EZ_FWD(tuple).for_each(EZ_FWD(f));
}

template <typename T, typename F1, typename F2, typename... Fs>
void for_each(T&& tuple, F1&& f1, F2&& f2, Fs&&... fs)
{
    ez::tuple::for_each(EZ_FWD(tuple), Overload{EZ_FWD(f1), EZ_FWD(f2), EZ_FWD(fs)...});
}

template <typename F, typename... Ts>
decltype(auto) apply(F&& f, const Tuple<Ts...>& tuple)
{
    return std::apply(EZ_FWD(f), static_cast<const std::tuple<Ts...>&>(tuple));
}

template <typename F, typename... Ts>
decltype(auto) apply(F&& f, Tuple<Ts...>& tuple)
{
    return std::apply(EZ_FWD(f), static_cast<std::tuple<Ts...>&>(tuple));
}

template <typename F, typename... Ts>
decltype(auto) apply(F&& f, Tuple<Ts...>&& tuple)
{
    return std::apply(EZ_FWD(f), static_cast<std::tuple<Ts...>&&>(tuple));
}

using std::apply;

}  // namespace tuple

}  // namespace ez

namespace std {

template <typename... Ts>
struct tuple_size<ez::Tuple<Ts...>> : integral_constant<size_t, sizeof...(Ts)> {
};

template <size_t I, typename... Ts>
struct tuple_element<I, ez::Tuple<Ts...>> : public tuple_element<I, std::tuple<Ts...>> {
};

template <size_t I, typename... Ts>
decltype(auto) get(const ez::Tuple<Ts...>& tuple)
{
    return std::get<I>(static_cast<const std::tuple<Ts...>&>(tuple));
}

template <size_t I, typename... Ts>
decltype(auto) get(ez::Tuple<Ts...>& tuple)
{
    return std::get<I>(static_cast<std::tuple<Ts...>&>(tuple));
}

template <size_t I, typename... Ts>
decltype(auto) get(ez::Tuple<Ts...>&& tuple)
{
    return std::get<I>(static_cast<std::tuple<Ts...>&&>(tuple));
}

}  // namespace std

namespace ez::tuple {
using std::get;
using std::tuple_element;
using std::tuple_size;
using std::tuple_size_v;
}  // namespace ez::tuple
