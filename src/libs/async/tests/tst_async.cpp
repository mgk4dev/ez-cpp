#include <gtest/gtest.h>

#include <ez/async/all.hpp>
#include <ez/scope_guard.hpp>
#include <ez/utils.hpp>

#include <boost/asio.hpp>

namespace asio = boost::asio;

namespace ez::async {

template <>
struct Executor<asio::thread_pool> {
    static void post(asio::thread_pool& executor, auto&& f)
    {
        boost::asio::post(executor, EZ_FWD(f));
    }
};

template <>
struct Executor<asio::io_context> {
    static void post(asio::io_context& executor, auto&& f)
    {
        boost::asio::post(executor, EZ_FWD(f));
    }
};

}  // namespace ez::async

using namespace ez;

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
    ASSERT_FALSE(co.is_ready());

    co.resume();

    ASSERT_TRUE(f_started);
    ASSERT_TRUE(body_started);
    ASSERT_TRUE(value_awaited);
    ASSERT_TRUE(co.is_ready());
}

TEST(Async, schedule_on_thread_pool)
{
    asio::thread_pool thread_pool1{1};
    asio::thread_pool thread_pool2{1};
    asio::thread_pool thread_pool3{1};

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
    asio::io_context executor;

    std::set<decltype(std::this_thread::get_id())> ids;

    auto task = [&]() -> Task<> {
        co_await async::schedule_on(executor);
        ids.insert(std::this_thread::get_id());
    };

    auto t = task();
    executor.run();
    ASSERT_TRUE(t.is_ready());
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
