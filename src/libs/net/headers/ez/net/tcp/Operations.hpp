#pragma once

#include <ez/net/Buffer.hpp>
#include <ez/net/ErrorCode.hpp>
#include <ez/net/tcp/Types.hpp>

#include <ez/async/Operation.hpp>
#include <ez/async/Task.hpp>

#include <ez/ByteArray.hpp>
#include <ez/Result.hpp>

#include <boost/asio/read.hpp>
#include <boost/asio/read_at.hpp>
#include <boost/asio/read_until.hpp>

#include <boost/asio/write.hpp>

namespace ez::net::tcp {

struct ConnectOp {
    Socket& scoket;
    const EndPoint& endpoint;
    ErrorCode error_code;

    bool done() const { return false; }
    void start(auto continuation)
    {
        scoket.async_connect(
            endpoint, [this, continuation = std::move(continuation)](ErrorCode error_code) mutable {
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

inline async::Operation<ConnectOp> async_connect(Socket& socket, const EndPoint& endpoint)
{
    return {socket, endpoint};
}

///////////////////////////////////////////////////////////////////////////////

struct AcceptOp {
    Acceptor& acceptor;
    Socket socket;
    ErrorCode error_code;

    AcceptOp(Acceptor& acceptor) : acceptor(acceptor), socket(acceptor.get_executor()) {}

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
    Result<Socket, ErrorCode> result()
    {
        if (error_code) return Fail{error_code};
        return Ok{std::move(socket)};
    }
};

inline async::Operation<AcceptOp> async_accept(Acceptor& acceptor) { return {acceptor}; }

///////////////////////////////////////////////////////////////////////////////

template <typename Buffer>
struct SendOp {
    Socket& socket;
    const Buffer& buffer;
    ErrorCode error_code;
    size_t count = 0;

    bool done() const { return false; }
    void start(auto continuation)
    {
        boost::asio::async_write(socket, buffer,
                                 [this, continuation = std::move(continuation)](
                                     ErrorCode error_code, size_t count) mutable {
                                     this->error_code = std::move(error_code);
                                     this->count = count;
                                     continuation();
                                 });
    }
    void cancel() {}
    Result<size_t, ErrorCode> result()
    {
        if (error_code) return Fail{error_code};
        return Ok{count};
    }
};

template <typename Buffer>
inline async::Operation<SendOp<Buffer>> async_send(Socket& socket, const Buffer& buffer)
{
    return {socket, buffer};
}

template <typename T>
inline async::Operation<SendOp<ConstBuffer>> async_send_pod(Socket& socket, const T& data)
{
    ConstBuffer buffer{&data, sizeof(T)};
    return {socket, buffer};
}

///////////////////////////////////////////////////////////////////////////////

struct ReceiveExactlyOp {
    Socket& socket;
    ByteArray& output;
    size_t count = 0;
    size_t received_count = 0;
    ErrorCode error_code;
    bool done_ = false;

    ReceiveExactlyOp(Socket& socket, ByteArray& output, size_t count)
        : socket{socket}, output{output}, count{count}
    {
        if (socket.available() >= count) {
            output.resize(count);
            received_count = boost::asio::read(socket, net::buffer(output),
                                               boost::asio::transfer_exactly(count), error_code);
            done_ = true;
        }
    }

    bool done() const { return done_; }

    void start(auto continuation)
    {
        output.resize(count);
        boost::asio::async_read(socket, net::buffer(output), boost::asio::transfer_exactly(count),
                                [this, continuation = std::move(continuation)](
                                    ErrorCode error_code, size_t count) mutable {
                                    this->error_code = std::move(error_code);
                                    this->received_count = count;
                                    continuation();
                                });
    }
    void cancel() {}
    Result<size_t, ErrorCode> result()
    {
        if (error_code) return Fail{error_code};
        return Ok{received_count};
    }
};

inline async::Operation<ReceiveExactlyOp> async_receive_exactly(Socket& socket,
                                                                ByteArray& output,
                                                                size_t count)
{
    return {socket, output, count};
}

///////////////////////////////////////////////////////////////////////////////

inline async::Task<Result<size_t, ErrorCode>> async_send_message(Socket& socket,
                                                                 const ConstBuffer& buffer)
{
    const auto size_bytes = ByteArray::from_pod_bytes<uint32_t>(buffer.size());
    const std::array<ConstBuffer, 2> data{net::buffer(size_bytes), buffer};
    co_return co_await async_send(socket, data);
}

inline async::Task<Result<size_t, ErrorCode>> async_receive_message(Socket& socket,
                                                                    ByteArray& output)
{
    ByteArray size_buffer;
    auto read_size = co_await async_receive_exactly(socket, size_buffer, sizeof(uint32_t));
    if (!read_size) co_return Fail{std::move(read_size.error())};
    co_return co_await async_receive_exactly(socket, output, size_buffer.to_pod_bytes<uint32_t>());
}

}  // namespace ez::net::tcp
