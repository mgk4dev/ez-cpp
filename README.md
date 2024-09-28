## ez-cpp
C++ tools and experimentation playground.

### Components
#### ez.core
Core C++ tools

#### ez.async
C++ coroutine library

#### ez.flow
Interpreter and virtual machine for the experimental flow async programming language.
### Build instructions
#### Requirements
* A C++20 compliant compiler
* CMake 3.2 or more recent
* Conan 2.x 

#### Building with CMake

```
mkdir build
cd build
conan install -of . --build missing -s build_type=<Debug> -r conan-center --profile <msvc2022> ../
cmake .. -DCMAKE_TOOLCHAIN_FILE="build/generators/conan_toolchain.cmake"
cmake -DCMAKE_BUILD_TYPE=<Debug> ..
cmake --build . --config <Debug> -j 8
```

### Snippets

#### Atomic
```C++
Atomic<std::vector<int>> vec;
const size_t size = 10000;

auto producer = [&vec](size_t count) {
    for (size_t i = 0; i < count; ++i) { vec->push_back(i); }
};

auto consumer = [&vec](size_t max) {
    size_t total = 0;
    while (total != max) {
        vec.edit([&](auto& vec) {
            if (!vec.empty()) {
                vec.pop_back();
                ++total;
            }
        });
    }
};

auto p1 = std::async(std::launch::async, producer, size);
auto p2 = std::async(std::launch::async, producer, size);
auto c = std::async(std::launch::async, consumer, size * 2);

p1.wait();
p2.wait();
c.wait();

ASSERT_TRUE(vec->empty());
```

#### Box
```C++
namespace box {

struct A {
    virtual ~A() = default;
    virtual int foo() const { return 0; }
};
struct B : public A {
    int foo() const { return 25; }
};
}  // namespace box

using box::A;
using box::B;
{
    Box<A> a{B{}};
    ASSERT_EQ(a->foo(), 25);
}

{
    Box<B> b;
    Box<A> a{std::move(b)};
    ASSERT_EQ(a->foo(), 25);
}

{
    Box<B> b;
    Box<A> a;
    a = std::move(b);
    ASSERT_EQ(a->foo(), 25);
}
```
#### Cow
```C++
Cow val = std::string{"Hello"};
auto val2 = val;
val = "Goodbye";
ASSERT_EQ(val2, "Hello");

```
#### Enum
```C++
Enum<int, double, std::string> val = 25;
ASSERT_TRUE(val.is<int>());
std::cout << val.as<int>();
val.match(
    [](int val)   { std::cout << "It is an int : " << val ; }
    [](double val){ std::cout << "It is an double : " << val ; }
    [](auto&& val){ std::cout << "It is something else " ; }
);
```

#### For constexpr
```C++
auto tuple = std::make_tuple(10, 10, 10);

for_constexpr<0, 3>([&](auto index) { std::get<index.value>(tuple) = index.value; });

ASSERT_EQ(std::get<0>(tuple), 0);
ASSERT_EQ(std::get<1>(tuple), 1);
ASSERT_EQ(std::get<2>(tuple), 2);
```

#### Result
```C++
enum class Error { Cause1, Cause2 };
auto task = []() -> Result<std::string, Error> { return Ok{"hello"}; };
auto result = task();
ASSERT_TRUE(result);
ASSERT_EQ(result.value(), "hello");

auto task = []() -> Result<std::string, Error> { return Fail{Error::Cause1}; };
auto result = task();
ASSERT_FALSE(result);
ASSERT_TRUE(result.is_error());
ASSERT_EQ(result.error(), Error::Cause1);
```

#### Scope guard
```C++
int counter = 0;
{
     EZ_ON_SCOPE_EXIT { ++counter; };
}
ASSERT_EQ(counter, 1);
```

#### Strong type
```C++
using Width = StrongType<int, struct WidthTag, mixin::NumberSemantic>;
using Height = StrongType<int, struct HeightTag, mixin::NumberSemantic>;
void resize(Widget& w, Width width, Height height);
Width width  = 10;
Height height = 20;

// The following will not compile !!
resize(w, height, width);  
width = height;
```

#### Tuple
```C++
Tuple tuple{1, std::string{"hello"}};
auto& [v1, v2] = tuple;
v1 = 10;
v2 = "toto";
ASSERT_EQ(tuple[constexpr_<0>], 10);
ASSERT_EQ(tuple[constexpr_<1>], "toto");
```

#### Async schedule on thread pool
```C++
ThreadPool thread_pool1{1};
ThreadPool thread_pool2{1};
ThreadPool thread_pool3{1};

std::set<decltype(std::this_thread::get_id())> ids;

auto task = [&]() -> Task<int> {
    co_await async::schedule_on(thread_pool1);
    ids.insert(std::this_thread::get_id());

    co_await async::schedule_on(thread_pool2);
    ids.insert(std::this_thread::get_id());

    co_await async::schedule_on(thread_pool3);
    ids.insert(std::this_thread::get_id());

    co_return 10;
};

auto t = task();

auto result = async::sync_wait(std::move(t));

ASSERT_EQ(ids.size(), 3);
ASSERT_EQ(result, 10);

ASSERT_FALSE(ids.contains(std::this_thread::get_id()));
```

#### Async when_all
```C++
auto task = [&](int ret) -> Task<int> { co_return ret; };

auto id1 = task(10);
auto id2 = task(20);

auto result = async::when_all(std::move(id1), std::move(id2));

auto [r1, r2] = async::sync_wait(std::move(result));

ASSERT_EQ(r1, 10);
ASSERT_EQ(r2, 20);
```

#### Async when_any
```C++
IoContext context;
WorkGuard guard{context};

auto task = [&]() -> Task<> {
    auto id = co_await when_any(delay(context, 10ms, 1), delay(context, 100ms, 2));
    ASSERT_EQ(id, 1); 
    context.stop();
};


Shared t = task();
async::post(context, [t]() mutable {
    t->resume();
});

context.run();
```

#### Async delay
```C++
IoContext context;
WorkGuard guard{context};

auto task = [&]() -> Task<> {
    auto start = std::chrono::high_resolution_clock::now();
    co_await delay(context, 100ms, 10);
    auto elapsed = std::chrono::high_resolution_clock::now() - start;
    ASSERT_GE(elapsed, 100ms); 
    context.stop();
};

Shared t = task();
async::post(context, [t]() mutable { t->resume(); });

context.run();
```

#### Async race
```C++
IoContext context;
WorkGuard guard{context};

auto task = [&]() -> Task<> {
    auto id = co_await race(delay(context, 10ms, 1), delay(context, 100ms, 2));
    ASSERT_EQ(id, 1); 
    context.stop();
};

Shared t = task();
async::post(context, [t]() mutable { t->resume(); });

context.run();
```


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
