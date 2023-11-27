#pragma once

#include <boost/asio/io_context.hpp>
#include <boost/asio/post.hpp>
#include <boost/asio/thread_pool.hpp>

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

}  // namespace ez::async
