#include <gtest/gtest.h>

#include <ez/net/Address.hpp>
#include <ez/net/Buffer.hpp>
#include <ez/net/ErrorCode.hpp>
#include <ez/net/IoContext.hpp>

#include <ez/net/http/Operations.hpp>
#include <ez/net/tcp/Operations.hpp>
#include <ez/net/udp/Operations.hpp>

#include <ez/async/Scope.hpp>

#include <ez/Atomic.hpp>
#include <ez/ScopeGuard.hpp>
#include <ez/ByteArray.hpp>

#include <future>
#include <print>

using namespace ez;

static std::mutex cout_mutex{};

template <typename... Args>
void println_client(std::format_string<Args...> base, Args&&... args)
{
    std::lock_guard guard{cout_mutex};

    std::print("             [client] ");
    std::println(base, std::forward<Args>(args)...);
}

template <typename... Args>
void println_server(std::format_string<Args...> base, Args&&... args)
{
    std::lock_guard guard{cout_mutex};

    std::print("[server] ");
    std::println(base, std::forward<Args>(args)...);
}

async::Task<> process_request(net::tcp::Socket socket)
{
    println_server("Reading request ...");

    while (socket.is_open()) {
        ByteArray buffer;

        if (auto received = co_await net::tcp::async_receive_message(socket, buffer); !received) {
            println_server("Failed to receive data from endpoint: {}", received.error().what());
            co_return;
        }
        else
            println_server("Received {} bytes", received.value());

        println_server("Received: '{}'", buffer.as_string());

        if (auto sent = co_await net::tcp::async_send_message(socket, net::buffer(buffer)); !sent) {
            println_server("Failed to send data to endpoint: {}", sent.error().what());
            co_return;
        }

        println_server("Processed request properly");
    }

    println_server("++++++++++++++ Closing");
}

void start_tcp_server(net::tcp::EndPoint endpoint)
{
    println_server(" Starting ...");

    net::IoContext context;

    async::Scope scope{context};

    net::tcp::Acceptor acceptor{context, endpoint};

    scope << [&] -> async::Task<> {
        while (true) {
            auto socket = co_await net::tcp::async_accept(acceptor);

            if (socket.is_error()) {
                println_server("Failed to accept connection on {}:{}. Error: {}",
                               endpoint.address().to_string(), endpoint.port(),
                               socket.error().what());
                continue;
            }

            println_server("Accepted connection");

            scope << process_request(std::move(socket.value()));
        }
    };

    context.run();
}

void start_tcp_client(net::tcp::EndPoint endpoint)
{
    println_client("Starting ...");

    net::IoContext context;

    async::Scope scope{context};

    scope << [&] -> async::Task<> {
        net::tcp::Socket socket{context};
        if (auto ok = co_await net::tcp::async_connect(socket, endpoint); !ok) {
            println_client("Failed to connect to endpoint: {}", ok.error().what());
            co_return;
        }

        println_client("Connected !!!");

        int i = 1;

        while (i < 3 && socket.is_open()) {
            EZ_ON_SCOPE_EXIT { ++i; };

            println_client("-------------------- Iteration {}", i);

            std::string data = std::format("Message {}", i);

            if (auto sent = co_await net::tcp::async_send_message(socket, net::buffer(data));
                !sent) {
                println_client("Failed to send data to endpoint: {}", sent.error().what());
                co_return;
            }
            else
                println_client("Sent request containing {} bytes", sent.value());

            ByteArray buffer;

            if (auto received = co_await net::tcp::async_receive_message(socket, buffer);
                !received) {
                println_client("Failed to receive data from endpoint: {}", received.error().what());
                co_return;
            }
            else
                println_client("Received message of {} bytes", received.value());

            println_client("Received: '{}'", buffer.as_string());
        }

        println_client("-------------------- Client done");
    };

    context.run();
}

TEST(Net, tcp_client_server)
{
    net::tcp::EndPoint endpoint{net::IpAddressV4::from_string("127.0.0.1"), 8080};
    auto client = std::async([endpoint] {
        std::this_thread::sleep_for(std::chrono::seconds(1));
        start_tcp_client(endpoint);
    });
    start_tcp_server(endpoint);
    client.wait();
}
