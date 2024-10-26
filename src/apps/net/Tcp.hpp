#pragma once

#include "Http.hpp"
#include "Websockets.hpp"

namespace ez::net {

inline async::Task<> handle_request(net::TaskPool& task_pool, tcp::Stream stream)
{
    auto request = co_await async_http_read(stream);

    if (request.is_error()) {
        println("Failed to read http request: {}", request.error().to_string());
        co_return;
    }

    auto& req = request.value();

    if (websocket::is_upgrade(req)) {
        co_await handle_websocket(task_pool, websocket::Stream{stream.release_socket()},
                                  std::move(req));
    }
    else {
        co_await handle_http_request(task_pool, std::move(stream), std::move(req));
    }
}

inline async::Task<> start_server(net::TaskPool& task_pool, tcp::EndPoint endpoint)
{
    println("Starting server on {}:{}", endpoint.address().to_string(), endpoint.port());
    tcp::Acceptor acceptor{task_pool.context(), endpoint};

    while (true) {
        auto socket = co_await async_accept(acceptor);

        if (socket.is_error()) {
            println("Failed to accept connection on {}:{}. Error: {}",
                    endpoint.address().to_string(), endpoint.port(), socket.error().to_string());
            continue;
        }

        task_pool << handle_request(task_pool, tcp::Stream{std::move(socket.value())});
    }
}

}  // namespace ez::net
