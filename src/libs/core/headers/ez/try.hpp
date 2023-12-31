#pragma once

#include <ez/preprocessor.hpp>
#include <ez/result.hpp>

#define EZ_TRY(target, monad)                                                                     \
    auto&& EZ_CONCAT(__monad, __LINE__) = monad;                                                  \
    if (!EZ_CONCAT(__monad, __LINE__))                                                            \
        return std::forward<decltype(EZ_CONCAT(__monad, __LINE__))>(EZ_CONCAT(__monad, __LINE__)) \
            .wrapped_error();                                                                     \
    target = EZ_CONCAT(__monad, __LINE__).value()
