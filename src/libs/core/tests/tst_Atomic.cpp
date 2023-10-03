#include <gtest/gtest.h>

#include <ez/Atomic.hpp>

#include <future>

using namespace ez;

TEST(Atomic, ctors)
{
    using AtomicString = Atomic<std::string>;

    AtomicString s0;
    ASSERT_EQ(s0.read(std::identity{}), "");

    AtomicString s1{"hello"};
    ASSERT_EQ(s1.read(std::identity{}), "hello");

    AtomicString s2{s1};
    ASSERT_EQ(s2.read(std::identity{}), "hello");

    AtomicString s3{std::move(s1)};
    ASSERT_EQ(s3.read(std::identity{}), "hello");
    ASSERT_EQ(s1.read(std::identity{}), "");

    s1 = "hello";
    AtomicString s4;
    s4 = s1;
    ASSERT_EQ(s4.read(std::identity{}), "hello");

    AtomicString s5;
    s5 = std::move(s1);
    ASSERT_EQ(s5.read(std::identity{}), "hello");
    ASSERT_EQ(s1.read(std::identity{}), "");

    AtomicString s6{4, '='};
    ASSERT_EQ(s6.read(std::identity{}), "====");
}

TEST(Atomic, read)
{
    Atomic<std::vector<int>> vec{std::vector{1, 2, 3}};

    size_t size = vec.read([](auto& v) { return v.size(); });

    ASSERT_EQ(size, 3);
}

TEST(Atomic, edit)
{
    Atomic<std::vector<int>> vec{std::vector{1, 2, 3}};

    size_t size = vec.edit([](auto& v) {
        v.push_back(4);
        return v.size();
    });

    ASSERT_EQ(size, 4);
}

TEST(Atomic, concurrent_access)
{
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
}
