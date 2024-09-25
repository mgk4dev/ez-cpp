#pragma once

#include <ez/Enum.hpp>
#include <ez/Tuple.hpp>

namespace ez {
template <typename T>
struct NamedArgument {
    template <typename U>
    constexpr T operator=(U&& val) const
    {
        return T{std::forward<U>(val)};
    }
};

template <typename Tuple, typename... Fs>
void dispatch_args(Tuple&& tuple, Fs&&... fs)
{
    EZ_FWD(tuple).template for_each(Overload{EZ_FWD(fs)...});
}

}  // namespace ez
