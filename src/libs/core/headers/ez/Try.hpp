#pragma once

#include <ez/Preprocessor.hpp>
#include <ez/Result.hpp>
#include <ez/Trait.hpp>

#include <optional>

namespace ez {

template <typename T>
decltype(auto) get_non_value(T&& monad)
{
    using TT = std::decay_t<T>;

    if constexpr (trait::IsTemplate<TT, Result>) { return std::forward<T>(monad).wrapped_error(); }
    else if constexpr (trait::IsTemplate<TT, std::optional>) {
        return std::nullopt;
    }
}

}  // namespace ez

#define EZ_TRY(T, val, monad)                                                                    \
    auto&& EZ_CONCAT(__monad, __LINE__) = monad;                                                 \
    if (!EZ_CONCAT(__monad, __LINE__))                                                           \
        return ez::get_non_value(                                                                \
            std::forward<decltype(EZ_CONCAT(__monad, __LINE__))>(EZ_CONCAT(__monad, __LINE__))); \
    T val = EZ_CONCAT(__monad, __LINE__).value()
