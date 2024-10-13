#pragma once

#include <ez/Utils.hpp>

#include <optional>

namespace ez {
struct None {};

inline constexpr None none{};

template <typename T>
class Option : public std::optional<T> {
public:
    using std::optional<T>::optional;
    using std::optional<T>::value;
    using std::optional<T>::has_value;

    Option(None) : std::optional<T>{std::nullopt} {}

    T operator|(auto&& val) const&
    {
        if (has_value()) return value();
        return EZ_FWD(val);
    }

    T operator|(auto&& val) &&
    {
        if (has_value()) return std::move(*this).value();
        return EZ_FWD(val);
    }
};

template <typename T>
Option(T) -> Option<T>;

}  // namespace ez
