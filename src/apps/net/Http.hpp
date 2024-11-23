#pragma once

#include "Websockets.hpp"

#include <print>

namespace ez::net {

inline async::Task<> handle_http_request(net::Scope&, tcp::Stream stream, http::Request request)
{
    std::println("Request = {} : {}", request.method_string().data(), request.target().data());

    http::Response response;

    response.set(http::field::server, "Test HTTP server");
    response.set(http::field::content_type, "text/json");
    response.keep_alive(request.keep_alive());
    response.body() = R"({
    "foo"  : "bar",
    "toto" : 100,
    "ok"   : true
}
)";
    response.prepare_payload();

    auto ok = co_await async_http_write(stream, response);

    if (!ok) { println("Failed to write http response: {}", ok.error().to_string()); }
}

}  // namespace ez::net
