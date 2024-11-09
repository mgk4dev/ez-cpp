#pragma once

#include <ez/async/Executor.hpp>

#include <boost/asio/io_context.hpp>
#include <boost/asio/post.hpp>

namespace ez::net {

using IoContext = boost::asio::io_context;

}

namespace ez::async {

template <>
struct Executor<net::IoContext> {
    template <typename T>
    static void post(ez::net::IoContext& context, T&& task)
    {
        boost::asio::post(context, std::forward<T>(task));
    }
};

}  // namespace ez::async
