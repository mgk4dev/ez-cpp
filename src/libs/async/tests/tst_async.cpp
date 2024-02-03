#include <gtest/gtest.h>

#include <ez/async/all.hpp>

#include <ez/scope_guard.hpp>
#include <ez/shared.hpp>
#include <ez/utils.hpp>

#include <thread>

using namespace ez;
using namespace ez::async;

using namespace std::chrono_literals;

TEST(Async, task)
{
    bool f_started = false;
    bool body_started = false;
    bool value_awaited = false;

    auto f = [&]() -> Task<int> {
        f_started = true;
        co_return 21;
    };

    auto body = [&]() -> Task<void> {
        body_started = true;
        int val = co_await f();
        unused(val);
        value_awaited = true;
    };

    auto co = body();

    ASSERT_FALSE(f_started);
    ASSERT_FALSE(body_started);
    ASSERT_FALSE(value_awaited);
    ASSERT_FALSE(co.done());

    co.resume();

    ASSERT_TRUE(f_started);
    ASSERT_TRUE(body_started);
    ASSERT_TRUE(value_awaited);
    ASSERT_TRUE(co.done());
}

TEST(Async, schedule_on_thread_pool)
{
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
}

TEST(Async, schedule_on_io_context)
{
    IoContext executor;

    std::set<decltype(std::this_thread::get_id())> ids;

    auto task = [&]() -> Task<> {
        co_await async::schedule_on(executor);
        ids.insert(std::this_thread::get_id());
    };

    auto t = task();
    t.resume();
    executor.run();
    ASSERT_TRUE(t.done());
    ASSERT_EQ(ids.size(), 1);
    ASSERT_TRUE(ids.contains(std::this_thread::get_id()));
}

TEST(Async, when_all)
{
    auto task = [&](int ret) -> Task<int> { co_return ret; };

    auto id1 = task(10);
    auto id2 = task(20);

    auto result = async::when_all(std::move(id1), std::move(id2));

    auto [r1, r2] = async::sync_wait(std::move(result));

    ASSERT_EQ(r1, 10);
    ASSERT_EQ(r2, 20);
}

TEST(Async, delay)
{
    IoContext context;
    WorkGuard guard{context};

    auto task = [&]() -> Task<> {
        auto start = std::chrono::high_resolution_clock::now();
        co_await delay(context, 100ms, 10);
        auto elapsed = std::chrono::high_resolution_clock::now() - start;

        [&] { ASSERT_GE(elapsed, 100ms); }();
        context.stop();
    };

    Shared t = task();
    async::post(context, [t]() mutable { t->resume(); });

    context.run();
}

TEST(Async, when_any)
{
    IoContext context;
    WorkGuard guard{context};

    auto task1 = [&]() -> Task<> {
        auto id = co_await when_any(delay(context, 10ms, 1), delay(context, 100ms, 2));
        [&] { ASSERT_EQ(id, 1); }();
        context.stop();
    };

    auto task2 = [&]() -> Task<> {
        auto id = co_await when_any(delay(context, 10ms, std::string{"task 1"}),
                                    delay(context, 100ms, 2));

        [&] {
            ASSERT_TRUE(id.is<std::string>());
            ASSERT_EQ(id.as<std::string>(), std::string{"task 1"});
        }();
        context.stop();
    };

    Shared t1 = task1();
    Shared t2 = task2();
    async::post(context, [t1, t2]() mutable {
        t1->resume();
        t2->resume();
    });

    context.run();
}

TEST(Async, race)
{
    IoContext context;
    WorkGuard guard{context};

    auto task = [&]() -> Task<> {
        auto id = co_await race(delay(context, 10ms, 1), delay(context, 100ms, 2));
        [&] { ASSERT_EQ(id, 1); }();

        context.stop();
    };

    Shared t = task();
    async::post(context, [t]() mutable { t->resume(); });

    context.run();
}

TEST(Async, when_any_throw)
{
    IoContext context;
    WorkGuard guard{context};

    auto work = [&]() -> Task<int> {
        co_await delay(context, 100ms, 10);
        throw std::runtime_error{""};
        co_return 0;
    };

    auto task = [&]() -> Task<> {
        EZ_ON_SCOPE_EXIT { context.stop(); };
        auto w = work();
        co_await when_any(w, delay(context, 1s, 10));
    };

    Shared t = task();
    async::post(context, [t]() mutable { t->resume(); });
    context.run();
}

TEST(Async, repeat_delay)
{
    IoContext context;
    WorkGuard guard{context};

    auto work = [&](uint count) -> Task<> {
        while (count--) { co_await delay(context, 1ms, 10); }
        context.stop();
    };

    Shared t = work(2000);
    async::post(context, [t]() mutable { t->resume(); });
    context.run();
}

struct FinalAwaiter {
    bool await_ready() noexcept { return false; }
    Coroutine<> await_suspend(auto current) noexcept { return current.promise().next; }
    void await_resume() noexcept {}
};

struct ReturnObject {
    struct promise_type {
        int result;
        std::exception_ptr exception = nullptr;
        bool has_value = false;

        std::coroutine_handle<> next = std::noop_coroutine();

        std::suspend_always initial_suspend() const noexcept { return {}; }

        FinalAwaiter final_suspend() const noexcept { return {}; }

        void return_value(int val)
        {
            result = val;
            has_value = true;
        }
        void unhandled_exception()
        {
            exception = std::current_exception();
            has_value = true;
        }
        ReturnObject get_return_object() noexcept
        {
            return {std::coroutine_handle<promise_type>::from_promise(*this)};
        }
    };

    ReturnObject(std::coroutine_handle<promise_type> handle) : m_handle{handle} {}

    ReturnObject(const ReturnObject&) = delete;
    ReturnObject& operator=(const ReturnObject&) = delete;

    ReturnObject(ReturnObject&& rhs) { std::swap(m_handle, rhs.m_handle); }
    ReturnObject& operator=(ReturnObject&& rhs)
    {
        std::swap(m_handle, rhs.m_handle);
        return *this;
    }

    ~ReturnObject() { m_handle.destroy(); }

    bool await_ready() noexcept { return m_handle.promise().has_value; }
    std::coroutine_handle<> await_suspend(auto caller) noexcept
    {
        m_handle.promise().next = caller;
        return m_handle;
    }
    int await_resume() noexcept { return m_handle.promise().result; }
    void resume() { m_handle.resume(); }

private:
    std::coroutine_handle<promise_type> m_handle;
};

ReturnObject sum(int a, int b)
{
    std::cout << "Executing sum" << std::endl;
    co_return a + b;
}

ReturnObject test()
{
    std::cout << "Executing test" << std::endl;
    auto s = sum(10, 20);
    int result = co_await s;
    std::cout << "Sum is " << result << std::endl;
    co_return result;
}

TEST(Async, demo)
{
    auto coco = test();
    coco.resume();
}



