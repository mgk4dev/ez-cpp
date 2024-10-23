
#### Remote Procedure Call
```C++

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
    auto& remote = remote_service.schema<SchemaV2>();

    auto foo = co_await remote.get_foo();
    std::cout << "Result is '" << foo.value() << "'" << std::endl;

    auto pong = co_await remote.ping(MyProtobufMessage{"hello"});
    std::cout << "Ping response is '" << pong.value().data << "'" << std::endl;
}

async::IoContext service_context, client_context;

Service service{service_context, transport.make_server()};
service.schema<SchemaV1>().get_foo = []() -> async::Task<std::string> { co_return "foo 1"; };
service.schema<SchemaV2>().get_foo = []() -> async::Task<std::string> { co_return "foo 2"; };

service.schema<SchemaV1>().ping = [](MyProtobufMessage msg) -> MyProtobufMessage {
    msg.data = "pong 1 " + msg.data;
    return msg;
};

service.schema<SchemaV2>().ping = [](MyProtobufMessage msg) -> MyProtobufMessage {
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

    task_pool.post(make_task(remote_service));

    client_context.run();
});

server_task.wait();
client_task.wait();

```
