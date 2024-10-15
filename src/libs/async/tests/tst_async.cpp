#include <gtest/gtest.h>

#include <ez/async/All.hpp>

#include <ez/ScopeGuard.hpp>
#include <ez/Utils.hpp>

#include "qadapters/QDelay.hpp"
#include "qadapters/QExecutor.hpp"

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
    QThreadPool thread_pool1;
    QThreadPool thread_pool2;
    QThreadPool thread_pool3;

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
    std::set<decltype(std::this_thread::get_id())> ids;

    auto task = [&]() -> Task<> {
        co_await async::schedule_on(qapp());
        ids.insert(std::this_thread::get_id());
    };

    auto t = task();
    t.resume();
    qapp().processEvents();
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

TEST(Async, when_any)
{
    TaskPool<QCoreApplication> task_pool{qapp()};

    size_t finished_count = 0;

    auto maybe_stop_app = [&] {
        ++finished_count;
        if (finished_count == 2) qapp().exit();
    };

    auto task1 = [&]() -> Task<> {
        EZ_ON_SCOPE_EXIT { maybe_stop_app(); };

        auto id = co_await when_any(qdelay(10ms, 1), qdelay(100ms, 2));
        [&] { ASSERT_EQ(id, 1); }();
    };

    auto task2 = [&]() -> Task<> {
        EZ_ON_SCOPE_EXIT { maybe_stop_app(); };

        auto id = co_await when_any(qdelay(10ms, std::string{"task 1"}), qdelay(100ms, 2));

        [&] {
            ASSERT_TRUE(id.is<std::string>());
            ASSERT_EQ(id.as<std::string>(), std::string{"task 1"});
        }();

        maybe_stop_app();
    };

    task_pool << task1() << task2();

    qapp().exec();
}

TEST(Async, delay)
{
    TaskPool<QCoreApplication> task_pool{qapp()};
    auto task = [&]() -> Task<> {
        EZ_ON_SCOPE_EXIT { qapp().exit(); };
        auto start = std::chrono::high_resolution_clock::now();
        co_await qdelay(100ms, 10);
        auto elapsed = std::chrono::high_resolution_clock::now() - start;

        [&] { ASSERT_GE(elapsed, 100ms); }();
    };

    task_pool << task();

    qapp().exec();
}

// TEST(Async, race)
// {
//     TaskPool<QCoreApplication> task_pool{qapp()};

//     auto task = [&]() -> Task<> {
//         auto id = co_await race(qdelay(10ms, 1), qdelay(100ms, 2));
//         [&] { ASSERT_EQ(id, 1); }();
//         qapp().exit();
//     };

//     task_pool << task();

//     qapp().exec();
// }

TEST(Async, when_any_throw)
{
    TaskPool<QCoreApplication> task_pool{qapp()};

    auto work = [&]() -> Task<int> {
        co_await qdelay(100ms, 10);
        throw std::runtime_error{""};
        co_return 0;
    };

    auto task = [&]() -> Task<> {
        EZ_ON_SCOPE_EXIT { qapp().exit(); };
        auto w = work();
        co_await when_any(w, qdelay(1s, 10));
    };

    task_pool << task();

    qapp().exec();
}

TEST(Async, repeat_delay)
{
    TaskPool<QCoreApplication> task_pool{qapp()};

    auto work = [&](unsigned count) -> Task<> {
        EZ_ON_SCOPE_EXIT { qapp().exit(); };

        while (count--) { co_await qdelay(1ms, 10); }
    };

    task_pool << work(200);

    qapp().exec();
}
