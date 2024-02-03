#pragma once

#include <ez/rpc/types.hpp>

namespace ez::rpc {

RequestId make_request_id();

struct Request {
    RequestId request_id;
    std::string name_space;
    std::string function_name;
    std::vector<ByteArray> arguments;
};

enum class ReplyType { Value, Error };

struct Reply {
    RequestId request_id;
    ByteArray result;
    ReplyType type;
};

inline bool operator==(const Request& lhs, const Request& rhs)
{
    return lhs.request_id == rhs.request_id && lhs.function_name == rhs.function_name &&
           std::equal(lhs.arguments.begin(), lhs.arguments.end(), rhs.arguments.begin(),
                      rhs.arguments.end());
}

inline bool operator==(const Reply& lhs, const Reply& rhs)
{
    return lhs.request_id == rhs.request_id && lhs.result == rhs.result;
}

}  // namespace ez::rpc
