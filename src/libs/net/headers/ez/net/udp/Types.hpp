#pragma once

#include <boost/asio/ip/udp.hpp>

namespace ez::net::udp {

using EndPoint = boost::asio::ip::udp::endpoint;
using Socket = boost::asio::ip::udp::socket;

inline const auto v4 = boost::asio::ip::udp::v4;

}  // namespace ez::net::udp
