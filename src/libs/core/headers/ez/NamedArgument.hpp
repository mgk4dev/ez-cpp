#pragma once

#include <utility>

namespace ez {
template <typename T>
struct NamedArgument {
    template <typename U>
    constexpr T operator=(U&& val) const
    {
        return T{std::forward<U>(val)};
    }
};

}  // namespace ez
