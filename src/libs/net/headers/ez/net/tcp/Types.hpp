#pragma once

#include <boost/asio/ip/tcp.hpp>

namespace ez::net::tcp {

using EndPoint = boost::asio::ip::tcp::endpoint;
using Acceptor = boost::asio::ip::tcp::acceptor;
using Socket = boost::asio::ip::tcp::socket;
using Stream = boost::asio::ip::tcp::iostream;

}  // namespace ez::net::tcp
