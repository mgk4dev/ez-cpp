#pragma once

#include <ez/async/executor.hpp>
#include <ez/async/trait.hpp>

#include <ez/shared.hpp>
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
        m_op.on_done([coroutine, done = m_done] () mutable {
            safe_resume(coroutine);
            done = true;
        });
    }

    auto await_resume() noexcept { return m_op.result(); }

    bool cancel()
    {
        if (m_done.value()) return false;
        m_op.cancel();
        m_done = true;
        return true;
    }

private:
    Ref<IoContext> m_context;
    Op m_op;
    Shared<bool> m_done = false;
};

template <typename T>
Operation(IoContext&, T&&) -> Operation<std::decay_t<T>>;


template <typename Op>
class ContextFreeOperation {
public:
    using ReturnType = decltype(std::declval<Op>().result());

    ContextFreeOperation(auto&&... args) : m_op{EZ_FWD(args)...} {}

    constexpr bool await_ready()
    {
        return m_op.is_ready();
    }

    void await_suspend(Coroutine<> coroutine)
    {
        m_op.on_done([coroutine, done = m_done] () mutable {
            safe_resume(coroutine);
            done = true;
        });
    }

    auto await_resume() noexcept { return m_op.result(); }

    bool cancel()
    {
        if (m_done.value()) return false;
        m_op.cancel();
        m_done = true;
        return true;
    }

private:
    Op m_op;
    Shared<bool> m_done = false;
};

template <typename T>
ContextFreeOperation(T&&) -> ContextFreeOperation<std::decay_t<T>>;

}  // namespace ez::async
