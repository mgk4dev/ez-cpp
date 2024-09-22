#pragma once

#include <ez/Preprocessor.hpp>

#include <utility>

namespace ez {

template <typename F>
struct ScopeExitAction {
    ~ScopeExitAction() { f(); }
    F f;
};

struct ScopeExitActionBuilder {
    template <typename F>
    auto operator<<(F&& func) -> ScopeExitAction<std::decay_t<F>>
    {
        return {std::forward<F>(func)};
    }
};

}  // namespace ez

#define EZ_ON_SCOPE_EXIT                                                             \
    auto EZ_ANONYMOUS_VARIABLE(ez_scope_exit_guard) = ::ez::ScopeExitActionBuilder{} \
                                                      << [&]() noexcept
