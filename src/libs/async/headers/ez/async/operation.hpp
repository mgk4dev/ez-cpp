#pragma once

#include <ez/async/executor.hpp>
#include <ez/async/trait.hpp>

#include <ez/utils.hpp>

namespace ez::async {

template <typename Op>
class Operation {
public:
    using ReturnType = decltype(std::declval<Op>().result());

    Operation(IoContext& context, auto&&... args) : m_context(context), m_op{EZ_FWD(args)...} {}

    constexpr bool await_ready()
    {
        m_op.start(m_context.get());
        return m_op.is_ready();
    }

    void await_suspend(Coroutine<> coroutine)
    {
        m_op.on_done([coroutine, this] {
            safe_resume(coroutine);
            m_done = true;
        });
    }

    auto await_resume() noexcept { return m_op.result(); }

    bool cancel()
    {
        if (m_done) return false;
        m_op.cancel();
        return m_done = true;
    }

private:
    Ref<IoContext> m_context;
    Op m_op;
    bool m_done = false;
};

template <typename T>
Operation(IoContext&, T&&) -> Operation<std::decay_t<T>>;

}  // namespace ez::async
