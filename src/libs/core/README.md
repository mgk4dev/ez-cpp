Core C++ tools

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

#### Rpl
Algorighm composition library
```C++
auto result = rpl::run(
    rpl::iota(1),
    rpl::transform([](int val) -> Option<int> { return (val % 2 == 0) ? Option<int>{val} : none; }),
    rpl::filter(),
    rpl::deref(),
    rpl::transform([](int val) { return val * val; }),
    rpl::take(3),
    rpl::to_vector()
);
ASSERT_EQ(result, (std::vector{2 * 2, 4 * 4, 6 * 6}));
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
