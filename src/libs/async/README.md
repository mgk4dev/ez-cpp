
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
