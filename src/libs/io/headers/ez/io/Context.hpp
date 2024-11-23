#pragma once

#include <ez/async/Executor.hpp>

#include <boost/asio/io_context.hpp>
#include <boost/asio/post.hpp>

namespace ez::io {
using Context = boost::asio::io_context;

class WorkGuard {
public:
    explicit WorkGuard(Context& ioContext) : m_guard{ioContext.get_executor()} {}
    void release() { m_guard.reset(); }

private:
    boost::asio::executor_work_guard<Context::executor_type> m_guard;
};
}  // namespace ez::io

namespace ez::async {

template <>
struct Executor<io::Context> {
    template <typename T>
    static void post(ez::io::Context& context, T&& task)
    {
        boost::asio::post(context, std::forward<T>(task));
    }
};

}  // namespace ez::async
