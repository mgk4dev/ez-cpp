#pragma once

#include <ez/async/task.hpp>

#include <ez/shared.hpp>

#include <boost/asio/io_context.hpp>
#include <boost/asio/post.hpp>
#include <boost/asio/thread_pool.hpp>

#include <queue>

namespace ez::async {

using IoContext = boost::asio::io_context;
class WorkGuard {
public:
    explicit WorkGuard(IoContext& ioContext) : m_guard{ioContext.get_executor()} {}
    void release() { m_guard.reset(); }

private:
    boost::asio::executor_work_guard<IoContext::executor_type> m_guard;
};

using ThreadPool = boost::asio::thread_pool;

using boost::asio::post;

class TaskPool {
public:
    TaskPool(IoContext& ctx) : m_context{ctx} {}

    void post(Shared<Task<>> task)
    {
        m_tasks.push(task);
        async::post(m_context.get(), [task]() mutable { task->resume(); });
    }

    TaskPool& operator<<(Shared<Task<>> task)
    {
        post(std::move(task));
        return *this;
    }

private:
    Ref<IoContext> m_context;
    std::queue<Shared<Task<>>> m_tasks;
};

}  // namespace ez::async
