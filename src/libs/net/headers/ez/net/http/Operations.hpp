#pragma once

#include <ez/net/Buffer.hpp>
#include <ez/net/ErrorCode.hpp>
#include <ez/net/http/Types.hpp>
#include <ez/net/tcp/Types.hpp>

#include <ez/Result.hpp>
#include <ez/async/Operation.hpp>

#include <boost/beast/http/read.hpp>
#include <boost/beast/http/write.hpp>

namespace ez::net::http {

// struct ReadOp {
//     tcp::Stream& stream;
//     ErrorCode error_code;
//     FlatBuffer buffer;
//     Request<StringBody> request;

//     bool done() const { return false; }
//     void start(auto continuation)
//     {
//         using boost::beast::http::async_read;
//         async_read(stream, buffer, request,
//                    [this, continuation = std::move(continuation)](ErrorCode error_code,
//                                                                   size_t /*count*/) mutable {
//                        this->error_code = std::move(error_code);
//                        continuation();
//                    });
//     }
//     void cancel() {}
//     Result<Request<StringBody>, ErrorCode> result()
//     {
//         if (error_code == http::error::end_of_stream) {
//             stream.socket().shutdown(tcp::Socket::shutdown_send, error_code);
//         }
//         if (error_code) return Fail{error_code};
//         return std::move(request);
//     }
// };

// inline async::Operation<ReadOp> async_read(tcp::Stream& stream) { return {stream}; }

/////////////////////////////////////////////////////////////////////

struct WriteOp {
    tcp::Stream& stream;
    const Response<StringBody>& response;
    ErrorCode error_code;

    bool done() const { return false; }
    void start(auto continuation)
    {
        using boost::beast::http::async_write;

        async_write(stream, response,
                    [this, continuation = std::move(continuation)](ErrorCode error_code,
                                                                   size_t /*count*/) mutable {
                        this->error_code = std::move(error_code);
                        continuation();
                    });
    }
    void cancel() {}

    Result<void, ErrorCode> result()
    {
        if (error_code) return Fail{error_code};
        return {};
    }
};

inline async::Operation<WriteOp> async_write(tcp::Stream& stream,
                                             const Response<StringBody>& response)
{
    return {stream, response};
}

}  // namespace ez::net::http
