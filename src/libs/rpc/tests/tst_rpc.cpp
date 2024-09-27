#include <gtest/gtest.h>

#include <ez/rpc/RemoteService.hpp>
#include <ez/rpc/Schema.hpp>
#include <ez/rpc/Service.hpp>

#include <ez/async/WhenAll.hpp>

#include <ez/Atomic.hpp>
#include <ez/Shared.hpp>

#include <format>
#include <future>
#include <queue>

using namespace ez;

struct Transport {
    using ClientMessageMap = std::unordered_map<rpc::PeerId, std::deque<rpc::ByteArray>>;
    using ServerMessagesMap =
        std::unordered_map<std::string, std::deque<rpc::transport::Server::Message>>;

    struct Messages {
        Atomic<ClientMessageMap> client;
        Atomic<ServerMessagesMap> server;
    };

    Messages messages;

    struct Client : public rpc::transport::Client {
        Ref<Messages> messages;
        rpc::PeerId id;
        std::string server_id;
        Client(Messages& messages, const rpc::PeerId& id) : messages{messages}, id{id} {}

        Result<void, std::runtime_error> connect(const std::string& server) final
        {
            server_id = server;
            return Ok{};
        }
        rpc::AsyncResult<> send(const rpc::ByteArray& data) final
        {
            std::cout << std::format("Client '{}' sending to '{}' : {}", id.value(), server_id,
                                     data.value())
                      << std::endl;

            messages.get().server->at(server_id).push_back({id, data});
            co_return Ok{};
        }
        bool receive(rpc::ByteArray& data) final
        {
            bool ok = false;
            messages.get().client.edit([&](ClientMessageMap& messages) {
                auto& my_queue = messages.at(id);
                if ((ok = !my_queue.empty())) {
                    data = std::move(my_queue.front());
                    my_queue.pop_front();
                }
            });

            if (ok) {
                std::cout << std::format("Client '{}' received from '{}' : {}", id.value(),
                                         server_id, data.value())
                          << std::endl;
            }

            return ok;
        }
    };

    struct Server : public rpc::transport::Server {
        Ref<Messages> messages;
        std::string id;
        Server(Messages& messages) : messages{messages} {}

        Result<void, std::runtime_error> bind_to(const std::string& id) final
        {
            this->id = id;
            messages.get().server->insert({id, {}});
            return Ok{};
        }
        rpc::AsyncResult<> send(const rpc::PeerId& peer_id, const rpc::ByteArray& payload) final
        {
            std::cout << std::format("Server '{}' sending to '{}' : {}", id, peer_id.value(),
                                     payload.value())
                      << std::endl;

            messages.get().client->at(peer_id).push_back(payload);
            co_return Ok{};
        }
        bool receive(Message& message) final
        {
            bool ok = false;
            messages.get().server.edit([&](ServerMessagesMap& messages) {
                auto& my_queue = messages.at(id);
                if ((ok = !my_queue.empty())) {
                    message = std::move(my_queue.front());
                    my_queue.pop_front();
                }
            });

            if (ok) {
                std::cout << std::format("Server '{}' received from '{}' : {}", id,
                                         message.peer_id.value(), message.payload.value())
                          << std::endl;
            }

            return ok;
        }
    };

    Box<rpc::transport::Client> make_client(const rpc::PeerId& id)
    {
        messages.client->insert(std::make_pair(id, std::deque<rpc::ByteArray>{}));
        return Client{messages, id};
    }
    Box<rpc::transport::Server> make_server() { return Server{messages}; }
};

struct MyProtobufMessage {
    std::string data;

    std::string SerializeAsString() const { return data; }

    bool ParseFromString(const std::string& data)
    {
        this->data = data;
        return true;
    }
};

struct SchemaV1 {
    EZ_RPC_NAMESAPCE(v1);

    rpc::Function<std::string()> get_foo{"get_foo"};
    rpc::Function<MyProtobufMessage(MyProtobufMessage)> ping{"ping"};
};

struct SchemaV2 {
    EZ_RPC_NAMESAPCE(v2);

    rpc::Function<std::string()> get_foo{"get_foo"};
    rpc::Function<MyProtobufMessage(MyProtobufMessage)> ping{"ping"};
};

using Service = rpc::Service<SchemaV1, SchemaV2>;
using RemoteService = rpc::RemoteService<SchemaV1, SchemaV2>;

void init_functions(SchemaV1& schema)
{
    schema.get_foo = []() -> async::Task<std::string> { co_return "foo 1"; };
    schema.ping = [](MyProtobufMessage msg) -> MyProtobufMessage {
        msg.data = "pong 1 " + msg.data;
        return msg;
    };
}

void init_functions(SchemaV2& schema)
{
    schema.get_foo = []() -> async::Task<std::string> { co_return "foo 2"; };

    schema.ping = [](MyProtobufMessage msg) -> MyProtobufMessage {
        msg.data = "pong 2 " + msg.data;
        return msg;
    };
}

auto make_task(RemoteService& remote_service) -> async::Task<>
{
    auto& remote = remote_service.functions<SchemaV2>();

    auto foo = co_await remote.get_foo();
    std::cout << "Result is '" << foo.value() << "'" << std::endl;

    auto pong = co_await remote.ping(MyProtobufMessage{"hello"});
    std::cout << "Ping response is '" << pong.value().data << "'" << std::endl;
}

TEST(Rpc, test)
{
    Transport transport;

    async::IoContext service_context, client_context;

    Service service{service_context, transport.make_server()};
    service.implementation<SchemaV1>().get_foo = []() -> async::Task<std::string> {
        co_return "foo 1";
    };
    service.implementation<SchemaV2>().get_foo = []() -> async::Task<std::string> {
        co_return "foo 2";
    };

    service.implementation<SchemaV1>().ping = [](MyProtobufMessage msg) -> MyProtobufMessage {
        msg.data = "pong 1 " + msg.data;
        return msg;
    };

    service.implementation<SchemaV2>().ping = [](MyProtobufMessage msg) -> MyProtobufMessage {
        msg.data = "pong 2 " + msg.data;
        return msg;
    };

    auto server_task = std::async([&] {
        async::WorkGuard guard{service_context};

        auto ok = service.bind_to("server 1");
        ASSERT_TRUE(ok);
        service.start();

        service_context.run();
    });

    auto client_task = std::async([&] {
        async::WorkGuard guard{client_context};

        RemoteService remote_service{client_context,
                                     transport.make_client(rpc::PeerId{"client 1"})};
        auto ok = remote_service.connect_to("server 1");
        ASSERT_TRUE(ok);
        remote_service.start();

        async::TaskPool task_pool{client_context};

        task_pool << make_task(remote_service);

        client_context.run();
    });

    server_task.wait();
    client_task.wait();
}
