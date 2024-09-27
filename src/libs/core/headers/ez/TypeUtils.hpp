#pragma once

#include <concepts>
#include <utility>

namespace ez {

template <typename... Ts>
struct TypeList;

template <typename T>
struct Type {
    using Inner = T;

    template <typename U>
    consteval bool operator==(Type<U>) const
    {
        return std::is_same_v<T, U>;
    }

    template <typename... Us>
    consteval auto operator+(TypeList<Us...>) const
    {
        return TypeList<T, Us...>{};
    }
};

template <typename T>
constexpr Type<T> type;

template <typename... Ts>
struct TypeList {
    static constexpr std::size_t count = sizeof...(Ts);

    template <template <typename...> typename T>
    using ApplyTo = T<Ts...>;

    using AddPtr = TypeList<Ts*...>;

    template <typename T>
    consteval bool contains(Type<T>) const
    {
        return (std::same_as<T, Ts> || ...);
    }

    template <typename... Us>
    consteval auto operator+(TypeList<Us...>) const
    {
        return TypeList<Ts..., Us...>{};
    }
};

namespace meta {

template <typename... Ts>
constexpr TypeList<Ts...> type_list;

void for_each(TypeList<>, auto&&) {}

template <typename T, typename... Ts>
void for_each(TypeList<T, Ts...>, auto&& f)
{
    f(type<T>);
    for_each(type_list<Ts...>, f);
}

template <typename T>
inline consteval auto remove_duplicates(TypeList<T>)
{
    return TypeList<T>{};
}

template <typename T, typename U, typename... Ts>
consteval auto remove_duplicates(TypeList<T, U, Ts...>)
{
    using Rhs = TypeList<U, Ts...>;

    if constexpr (Rhs{}.contains(type<T>)) { return remove_duplicates(Rhs{}); }
    else {
        return type<T> + remove_duplicates(TypeList<U, Ts...>{});
    }
}

template <typename T, typename... Ts>
consteval auto front_type(TypeList<T, Ts...>)
{
    return Type<T>{};
}
}  // namespace meta
}  // namespace ez
