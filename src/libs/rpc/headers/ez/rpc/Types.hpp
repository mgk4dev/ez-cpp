#pragma once

#include <vector>

#include <ez/async/Task.hpp>

#include <ez/Result.hpp>
#include <ez/StrongType.hpp>

namespace ez::rpc {
using ByteArray = StrongType<std::string, struct ByteArrayTag, mixin::Comparable>;
using PeerId = StrongType<std::string, struct PeerIdTag, mixin::Comparable, mixin::Hashable>;
using RequestId = StrongType<std::string, struct RequestIdTag, mixin::Comparable, mixin::Hashable>;

using ParsingError = std::runtime_error;

struct Error {
    // Keep the same values as in messages.proto.Error.Code
    enum Code : int { Timeout, FailedToSendRequest, FunctionNotFound, InternalError };

    Code code;
    std::string what;

    static Error send_request_failure()
    {
        return Error{FailedToSendRequest, "Failed to send request"};
    }
    static Error timeout() { return Error{Timeout, "Response timeout"}; }
    static Error internal_error(auto&& message) { return Error{InternalError, EZ_FWD(message)}; }
};

using RawReply = Result<ByteArray, rpc::Error>;

template <typename T = void>
using AsyncResult = async::Task<Result<T, rpc::Error>>;

}  // namespace ez::rpc
