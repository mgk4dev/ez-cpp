#pragma once

#include "Operations.hpp"

#include <ez/Print.hpp>

namespace ez::net {

inline async::Task<> handle_websocket(net::TaskPool& /*task_pool*/,
                                      websocket::Stream stream,
                                      http::Request /*request*/)
{
    auto ok = co_await net::async_accept(stream);

    if (!ok) {
        println("Failed to accept websocket: {}", ok.error().to_string());
        co_return;
    }

    auto buffer = co_await net::async_read(stream);

    if (!buffer) {
        println("Error reading from websocket: {}", buffer.error().to_string());
        co_return;
    }

    ok = co_await net::async_write(stream, buffer.value());

    if (!ok) {
        println("Failed to write to websocket: {}", ok.error().to_string());
        co_return;
    }
    co_return;
}
}  // namespace ez::net
