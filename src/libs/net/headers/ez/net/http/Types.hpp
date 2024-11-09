#pragma once

#include <boost/beast/http/error.hpp>
#include <boost/beast/http/field.hpp>
#include <boost/beast/http/string_body.hpp>
#include <boost/beast/http/message.hpp>


namespace ez::net::http {

using boost::beast::http::error;
using boost::beast::http::field;

using StringBody = boost::beast::http::string_body;

template <typename Body>
using Request = boost::beast::http::request<Body>;

template <typename Body>
using Response = boost::beast::http::response<Body>;

}  // namespace ez::net::http
