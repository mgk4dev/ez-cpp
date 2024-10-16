#pragma once

#include <vector>

#include <ez/async/Executor.hpp>
#include <ez/async/Task.hpp>

#include <ez/Result.hpp>
#include <ez/StrongType.hpp>

#include <boost/asio.hpp>

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

using IoContext = boost::asio::io_context;

}  // namespace ez::rpc

namespace ez::async {
template <>
struct Executor<rpc::IoContext> {
    static void post(rpc::IoContext& context, auto&& f) { boost::asio::post(context, EZ_FWD(f)); }
};

}  // namespace ez::async
