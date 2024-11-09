#pragma once

#include "Http.hpp"
#include "Websockets.hpp"

namespace ez::net {

inline async::Task<> handle_request(net::Scope& scope, tcp::Stream stream)
{
    auto request = co_await async_http_read(stream);

    if (request.is_error()) {
        println("Failed to read http request: {}", request.error().to_string());
        co_return;
    }

    auto& req = request.value();

    if (websocket::is_upgrade(req)) {
        co_await handle_websocket(scope, websocket::Stream{stream.release_socket()},
                                  std::move(req));
    }
    else {
        co_await handle_http_request(scope, std::move(stream), std::move(req));
    }
}

inline async::Task<> start_server(net::Scope& scope, tcp::EndPoint endpoint)
{
    println("Starting server on {}:{}", endpoint.address().to_string(), endpoint.port());
    tcp::Acceptor acceptor{scope.context(), endpoint};

    while (true) {
        auto socket = co_await async_accept(acceptor);

        if (socket.is_error()) {
            println("Failed to accept connection on {}:{}. Error: {}",
                    endpoint.address().to_string(), endpoint.port(), socket.error().to_string());
            continue;
        }

        scope << handle_request(scope, tcp::Stream{std::move(socket.value())});
    }
}

}  // namespace ez::net
