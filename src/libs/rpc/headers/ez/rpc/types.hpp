#pragma once

#include <vector>

#include <ez/async/task.hpp>

#include <ez/result.hpp>
#include <ez/strong_type.hpp>

namespace ez::rpc {

using ByteArray = StrongType<std::string, struct ByteArrayTag, mixin::Comparable>;
using PeerId = StrongType<std::string, struct PeerIdTag, mixin::Comparable, mixin::Hashable>;
using RequestId = StrongType<std::string, struct RequestIdTag, mixin::Comparable, mixin::Hashable>;

using ParsingError = std::runtime_error;

class Error : public std::runtime_error {
public:
    enum Kind { FailedToSendRequest, Timeout, RemoteError };

    Error(Kind k, auto&& message) : std::runtime_error{EZ_FWD(message)}, m_kind{k} {}

    static Error send_request_failure()
    {
        return Error{FailedToSendRequest, "Failed to send request"};
    }
    static Error timeout() { return Error{Timeout, "Response timeout"}; }
    static Error remote_error(auto&& message) { return Error{RemoteError, EZ_FWD(message)}; }

    Kind kind() const { return m_kind; }

private:
    Kind m_kind;
};

inline bool operator==(const Error& lhs, const Error& rhs)
{
    return lhs.kind() == rhs.kind() && std::string_view{lhs.what()} == std::string_view{rhs.what()};
}

template <typename T = void>
using AsyncResult = async::Task<Result<T, Error>>;

}  // namespace ez::rpc
