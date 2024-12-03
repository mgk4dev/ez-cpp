#pragma once

#include <ez/async/Executor.hpp>
#include <ez/async/Task.hpp>
#include <ez/async/Scope.hpp>

#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/tcp.hpp>

#include <boost/beast.hpp>

namespace ez::net {

using IoContext = boost::asio::io_context;
using Scope = async::Scope<IoContext>;

using ConstBuffer = boost::asio::const_buffer;
using MutableBuffer = boost::asio::mutable_buffer;
using boost::asio::buffer;
using boost::asio::post;

using ErrorCode = boost::system::error_code;

using Address = boost::asio::ip::address_v4;

using FlatBuffer = boost::beast::flat_buffer;

namespace tcp {

using EndPoint = boost::asio::ip::tcp::endpoint;
using Socket = boost::asio::ip::tcp::socket;
using Acceptor = boost::asio::ip::tcp::acceptor;

using Stream = boost::beast::tcp_stream;

}  // namespace tcp

namespace http {
using boost::beast::http::async_read;
using boost::beast::http::async_write;
using boost::beast::http::error;
using boost::beast::http::field;

using Stringbody = boost::beast::http::string_body;

using Request = boost::beast::http::request<Stringbody>;
using Response = boost::beast::http::response<Stringbody>;

}  // namespace http

namespace websocket {

using boost::beast::websocket::error;
using boost::beast::websocket::is_upgrade;

using Stream = boost::beast::websocket::stream<tcp::Socket>;

}  // namespace websocket

}  // namespace ez::net

namespace ez::async {

template <>
struct Executor<net::IoContext> {
    static void post(net::IoContext& context, auto&& task) { net::post(context, EZ_FWD(task)); }
};

}  // namespace ez::async
