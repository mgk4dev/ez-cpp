#pragma once

#include <ez/async/Executor.hpp>
#include <ez/async/Task.hpp>

#include <ez/Shared.hpp>

#include <algorithm>

#include <deque>

namespace ez::async {

template <typename Context>
class TaskPool {
public:
    TaskPool(Context& ctx) : m_context{ctx} {}
    TaskPool& operator<<(Task<> task);

private:
    void cleanup();

private:
    Ref<Context> m_context;
    std::deque<Task<>> m_tasks;
};

template <typename Context>
TaskPool<Context>& TaskPool<Context>::operator<<(Task<> task)
{
    cleanup();

    auto handle = task.handle();

    m_tasks.push_back(std::move(task));

    Executor<Context>::post(m_context.get(), [handle]() mutable { handle.resume(); });

    return *this;
}

template <typename Context>
void TaskPool<Context>::cleanup()
{
    auto ret = std::ranges::remove_if(m_tasks, [](auto& task) { return task.done(); });
    m_tasks.erase(ret.begin(), ret.end());
}

}  // namespace ez::async
