#pragma once

#include <ez/async/Executor.hpp>

#include <boost/asio/post.hpp>
#include <boost/asio/thread_pool.hpp>

namespace ez::io {
using ThreadPool = boost::asio::thread_pool;

}  // namespace ez::io

namespace ez::async {

template <>
struct Executor<io::ThreadPool> {
    template <typename T>
    static void post(io::ThreadPool& thread_pool, T&& task)
    {
        boost::asio::post(thread_pool, std::forward<T>(task));
    }
};

}  // namespace ez::async
