#pragma once

#include <boost/asio/buffer.hpp>
#include <boost/asio/streambuf.hpp>

#include <array>

namespace ez::net {

using boost::asio::buffer;

using ConstBuffer = boost::asio::const_buffer;
using MutableBuffer = boost::asio::mutable_buffer;

}  // namespace ez::net
