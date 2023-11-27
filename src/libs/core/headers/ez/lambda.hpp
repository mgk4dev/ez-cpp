#pragma once

#include <ez/utils.hpp>

namespace ez {

template <typename T>
struct Return {
    T val;

    constexpr T operator()(auto&&...) { return std::move(val); }
};

template <typename T>
Return(T&&) -> Return<std::decay_t<T>>;

///////////////////////////////////////////////////////////////////////////////

template <typename F>
struct Call {
    F f;
    constexpr decltype(auto) operator()(auto&&... arg) { return f(EZ_FWD(arg)...); }
};

template <typename F>
Call(F&&) -> Call<std::decay_t<F>>;

}  // namespace ez
