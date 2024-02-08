#include <ez/async/executor.hpp>

#include <algorithm>

namespace ez::async {

TaskPool& TaskPool::operator<<(Task<> task)
{
    auto wrap = [this](Task<> task) mutable -> Task<> {
        co_await task;
        post(m_context.get(), [this] { cleanup(); });
    };

    Shared task_wrapper = wrap(std::move(task));
    m_tasks.push_back(task_wrapper);

    post(m_context.get(), [task_wrapper]() mutable { task_wrapper->resume(); });

    return *this;
}

void TaskPool::cleanup()
{
    auto ret = std::ranges::remove_if(m_tasks, [](auto& task) { return task->done(); });
    m_tasks.erase(ret.begin(), ret.end());
}

}  // namespace ez::async
