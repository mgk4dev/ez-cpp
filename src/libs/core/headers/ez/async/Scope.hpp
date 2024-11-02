#pragma once

#include <ez/async/Executor.hpp>
#include <ez/async/Task.hpp>

#include <ez/Shared.hpp>
#include <ez/Traits.hpp>

#include <algorithm>

#include <deque>

namespace ez::async {

template <typename Context>
class Scope {
public:
    Scope(Context& ctx) : m_context{ctx} {}

    Context& context();
    const Context& context() const;

    Scope& operator<<(Task<> task);
    Scope& operator<<(trait::Fn<Task<>()> auto&& callable);

private:
    void cleanup();

private:
    Ref<Context> m_context;
    std::deque<Task<>> m_tasks;
};

///////////////////////////////////////////////////////////////////////////////

template <typename T>
Scope(T&) -> Scope<T>;

template <typename Context>
Scope<Context>& Scope<Context>::operator<<(Task<> task)
{
    cleanup();
    auto handle = task.handle();
    m_tasks.push_back(std::move(task));
    Executor<Context>::post(m_context.get(), [handle]() mutable { handle.resume(); });
    return *this;
}

template <typename Context>
Scope<Context>& Scope<Context>::operator<<(trait::Fn<Task<>()> auto&& callable)
{
    return *this << callable();
}

template <typename Context>
Context& Scope<Context>::context()
{
    return m_context;
}

template <typename Context>
const Context& Scope<Context>::context() const
{
    return m_context;
}

template <typename Context>
void Scope<Context>::cleanup()
{
    auto ret = std::ranges::remove_if(m_tasks, [](auto& task) { return task.done(); });
    m_tasks.erase(ret.begin(), ret.end());
}

}  // namespace ez::async
