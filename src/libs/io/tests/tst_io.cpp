#include <gtest/gtest.h>

#include <ez/io/Context.hpp>
#include <ez/io/Delay.hpp>

#include <ez/async/Race.hpp>
#include <ez/async/Scope.hpp>

using namespace ez;

using namespace std::chrono_literals;

TEST(Async, delay)
{
    io::Context context;
    io::WorkGuard guard{context};
    async::Scope scope{context};
    auto task = [&]() -> async::Task<> {
        auto start = std::chrono::high_resolution_clock::now();
        co_await io::delay(context, 100ms, 10);
        auto elapsed = std::chrono::high_resolution_clock::now() - start;

        [&] { ASSERT_GE(elapsed, 100ms); }();
        context.stop();
    };

    scope << task();

    context.run();
}

TEST(Async, when_any)
{
    io::Context context;
    io::WorkGuard guard{context};
    async::Scope scope{context};

    auto task1 = [&]() -> async::Task<> {
        auto id =
            co_await async::when_any(io::delay(context, 10ms, 1), io::delay(context, 100ms, 2));
        [&] { ASSERT_EQ(id, 1); }();
        context.stop();
    };

    auto task2 = [&]() -> async::Task<> {
        auto id = co_await async::when_any(io::delay(context, 10ms, std::string{"task 1"}),
                                           io::delay(context, 100ms, 2));

        [&] {
            ASSERT_TRUE(id.is<std::string>());
            ASSERT_EQ(id.as<std::string>(), std::string{"task 1"});
        }();
        context.stop();
    };

    scope << task1() << task2();

    context.run();
}

TEST(Async, race)
{
    io::Context context;
    io::WorkGuard guard{context};
    async::Scope scope{context};

    auto task = [&]() -> async::Task<> {
        auto id = co_await async::race(io::delay(context, 10ms, 1), io::delay(context, 100ms, 2));
        [&] { ASSERT_EQ(id, 1); }();

        context.stop();
    };

    scope << task();

    context.run();
}
