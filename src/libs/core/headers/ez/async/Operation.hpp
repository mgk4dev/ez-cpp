#pragma once

#include <ez/Shared.hpp>
#include <ez/async/Types.hpp>

namespace ez::async {

namespace traits {

// clang-format off
template <typename Op>
concept Operation = requires(Op& op){
    {op.done()} -> std::same_as<bool>;
    op.start([]{});
    op.result();
    op.cancel();

};
// clang-format off


}

template <traits::Operation Impl>
struct Operation {
    Impl impl;
    Shared<bool> m_done{false};

    Operation(auto&&... args) : impl{EZ_FWD(args)...} {}

    constexpr bool await_ready() { return impl.done(); }

    void await_suspend(CoHandle<> coroutine) {
        impl.start([coroutine, done = m_done]() mutable {
            safe_resume(coroutine);
            done = true;
        });
    }

    auto await_resume() noexcept
    {
        return impl.result();
    }

    bool cancel()
    {
        if (m_done.value()) return false;
        impl.cancel();
        m_done = true;
        return true;
    }
};

}  // namespace ez::async
