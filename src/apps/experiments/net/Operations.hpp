#pragma once

#include "Types.hpp"

#include <ez/async/Operation.hpp>

#include <ez/Option.hpp>
#include <ez/Result.hpp>

namespace ez::net {

struct AcceptOp {
    tcp::Acceptor& acceptor;
    tcp::Socket socket;
    ErrorCode error_code;

    AcceptOp(tcp::Acceptor& acceptor) : acceptor(acceptor), socket(acceptor.get_executor()) {}

    bool done() const { return false; }
    void start(auto continuation)
    {
        acceptor.async_accept(
            socket, [this, continuation = std::move(continuation)](ErrorCode error_code) mutable {
                this->error_code = std::move(error_code);
                continuation();
            });
    }
    void cancel() {}
    Result<tcp::Socket, ErrorCode> result()
    {
        if (error_code) return Fail{error_code};
        return Ok{std::move(socket)};
    }
};

inline async::Operation<AcceptOp> async_accept(tcp::Acceptor& acceptor) { return {acceptor}; }

/////////////////////////////////////////////////////////////////////

struct HttpReadOp {
    tcp::Stream& stream;
    ErrorCode error_code;
    FlatBuffer buffer;
    http::Request request;

    bool done() const { return false; }
    void start(auto continuation)
    {
        http::async_read(stream, buffer, request,
                         [this, continuation = std::move(continuation)](ErrorCode error_code,
                                                                        size_t /*count*/) mutable {
                             this->error_code = std::move(error_code);
                             continuation();
                         });
    }
    void cancel() {}
    Result<http::Request, ErrorCode> result()
    {
        if (error_code == http::error::end_of_stream) {
            stream.socket().shutdown(tcp::Socket::shutdown_send, error_code);
        }
        if (error_code) return Fail{error_code};
        return Ok{std::move(request)};
    }
};

inline async::Operation<HttpReadOp> async_http_read(tcp::Stream& stream) { return {stream}; }

/////////////////////////////////////////////////////////////////////

struct HttpWriteOp {
    tcp::Stream& stream;
    const http::Response& response;
    ErrorCode error_code;

    bool done() const { return false; }
    void start(auto continuation)
    {
        http::async_write(stream, response,
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
        return Ok{};
    }
};

inline async::Operation<HttpWriteOp> async_http_write(tcp::Stream& stream,
                                                      const http::Response& response)
{
    return {stream, response};
}

/////////////////////////////////////////////////////////////////////

struct AcceptWebsocketOp {
    websocket::Stream& stream;
    ErrorCode error_code;

    bool done() const { return false; }
    void start(auto continuation)
    {
        stream.async_accept(
            [this, continuation = std::move(continuation)](ErrorCode error_code) mutable {
                this->error_code = std::move(error_code);
                continuation();
            });
    }
    void cancel() {}

    Result<void, ErrorCode> result()
    {
        if (error_code) return Fail{error_code};
        return Ok{};
    }
};

inline async::Operation<AcceptWebsocketOp> async_accept(websocket::Stream& stream)
{
    return {stream};
}

/////////////////////////////////////////////////////////////////////

struct ReadWebsocketOp {
    websocket::Stream& stream;
    ErrorCode error_code;
    FlatBuffer buffer;

    bool done() const { return false; }
    void start(auto continuation)
    {
        stream.async_read(buffer, [this, continuation = std::move(continuation)](
                                      ErrorCode error_code, size_t /*count*/) mutable {
            this->error_code = std::move(error_code);
            continuation();
        });
    }
    void cancel() {}
    Result<FlatBuffer, ErrorCode> result()
    {
        if (error_code) return Fail{error_code};
        return Ok{std::move(buffer)};
    }
};

inline async::Operation<ReadWebsocketOp> async_read(websocket::Stream& stream) { return {stream}; }

/////////////////////////////////////////////////////////////////////

struct WriteWebsocketOp {
    websocket::Stream& stream;
    const FlatBuffer& buffer;
    ErrorCode error_code;

    bool done() const { return false; }
    void start(auto continuation)
    {
        stream.async_write(buffer.data(), [this, continuation = std::move(continuation)](
                                              ErrorCode error_code, size_t /*count*/) mutable {
            this->error_code = std::move(error_code);
            continuation();
        });
    }
    void cancel() {}
    Result<void, ErrorCode> result()
    {
        if (error_code) return Fail{error_code};
        return Ok{};
    }
};

inline async::Operation<WriteWebsocketOp> async_write(websocket::Stream& stream,
                                                      const FlatBuffer& buffer)
{
    return {stream, buffer};
}

}  // namespace ez::net
