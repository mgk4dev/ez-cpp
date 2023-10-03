#include <gtest/gtest.h>

#include <ez/async/Future.hpp>
#include <ez/async/Schedule.hpp>

#include <boost/asio.hpp>

namespace asio = boost::asio;

namespace ez::async {

template <>
struct Executor<asio::thread_pool> {
    template <typename F>
    static void post(asio::thread_pool& executor, F&& f)
    {
        boost::asio::post(executor, std::forward<F>(f));
    }
};

template <>
struct Executor<asio::io_context> {
    template <typename F>
    static void post(asio::io_context& executor, F&& f)
    {
        boost::asio::post(executor, std::forward<F>(f));
    }
};

}  // namespace ez::async

using namespace ez;

TEST(Schedule, thread_pool)
{
    asio::thread_pool thread_pool1{1};
    asio::thread_pool thread_pool2{1};
    asio::thread_pool thread_pool3{1};

    std::set<decltype(std::this_thread::get_id())> ids;

    auto task = [&]() -> Future<> {
        co_await async::schedule_on(thread_pool1);

        ids.insert(std::this_thread::get_id());

        co_await async::schedule_on(thread_pool2);
        ids.insert(std::this_thread::get_id());

        co_await async::schedule_on(thread_pool3);
        ids.insert(std::this_thread::get_id());
    };

    auto t = task();

    while (!t.is_ready()) { std::this_thread::sleep_for(std::chrono::milliseconds{10}); }

    ASSERT_EQ(ids.size(), 3);
    ASSERT_FALSE(ids.contains(std::this_thread::get_id()));
}

TEST(Schedule, io_context)
{
    asio::io_context executor;

    std::set<decltype(std::this_thread::get_id())> ids;

    auto task = [&]() -> Future<> {
        co_await async::schedule_on(executor);
        ids.insert(std::this_thread::get_id());
    };

    auto t = task();
    executor.run();
    ASSERT_TRUE(t.is_ready());
    ASSERT_EQ(ids.size(), 1);
    ASSERT_TRUE(ids.contains(std::this_thread::get_id()));
}
