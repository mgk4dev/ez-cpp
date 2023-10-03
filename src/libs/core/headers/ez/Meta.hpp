#pragma once

#include <concepts>
#include <utility>

namespace ez {

template <typename T>
struct Type {
    using Inner = T;

    template <typename U>
    constexpr bool operator==(Type<U>) const
    {
        return std::is_same_v<T, U>;
    }
};

template <typename T>
constexpr Type<T> type;

template <typename... Ts>
struct TypeList {
    static constexpr std::size_t count = sizeof...(Ts);

    template <typename T>
    consteval bool contains(Type<T>) const
    {
        return (std::same_as<T, Ts> || ...);
    }
};

template <typename... Ts>
constexpr TypeList<Ts...> type_list;

}  // namespace ez
