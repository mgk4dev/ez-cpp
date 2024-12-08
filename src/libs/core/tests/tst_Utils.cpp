#include <gtest/gtest.h>

#include <ez/Utils.hpp>

using namespace ez;

TEST(Utils, constexpr)
{
    auto is_2 = [](Constexpr<int> auto val) -> int {
        static_assert(val == 2);
        return val;
    };

    ASSERT_EQ(is_2(let<2>), 2);
}

TEST(ForConstexpr, iterate_values)
{
    int counter = 0;

    for_constexpr<0, 4>([&](auto index) { counter += index.value; });

    ASSERT_EQ(counter, 6);
}

TEST(ForConstexpr, size_index)
{
    int counter = 0;

    for_constexpr<size_t{0}, size_t{4}>([&](auto index) { counter += index.value; });

    ASSERT_EQ(counter, 6);
}

TEST(ForConstexpr, no_loop)
{
    int counter = 0;

    for_constexpr<0, 0>([&](auto index) { counter += index.value; });

    ASSERT_EQ(counter, 0);
}

TEST(ForConstexpr, access_tuple)
{
    auto tuple = std::make_tuple(10, 10, 10);

    for_constexpr<0, 3>([&](auto index) { std::get<index.value>(tuple) = index.value; });

    ASSERT_EQ(std::get<0>(tuple), 0);
    ASSERT_EQ(std::get<1>(tuple), 1);
    ASSERT_EQ(std::get<2>(tuple), 2);
}
