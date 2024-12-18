#include <gtest/gtest.h>

#include <ez/OneOf.hpp>

using namespace ez;

TEST(OneOf, case)
{
    auto f = Case<int> = [](auto&&) { return 1; };

    {
        int i{10};
        const auto result = f(i);
        ASSERT_EQ(result, 1);
    }

    {
        const int i{0};
        const auto result = f(i);
        ASSERT_EQ(result, 1);
    }

    {
        int&& i{10};
        const auto result = f(std::move(i));
        ASSERT_EQ(result, 1);
    }
}

TEST(OneOf, match)
{
    OneOf<int, double, std::string, std::vector<int>> val = "hello";

    // clang-format off
    auto index = val.match(
        [](int ) {return 0;},
        [](auto&& ) {return 4;}
    );
    // clang-format on

    ASSERT_EQ(index, 4);
}

TEST(OneOf, is)
{
    OneOf<int, double, std::string, std::vector<int>> val = "hello";
    ASSERT_TRUE(val.is<std::string>());
}

TEST(OneOf, as)
{
    OneOf<int, double, std::string, std::vector<int>> val = 258;

    ASSERT_TRUE(val.is<int>());

    val.as<int>() = 0;
    ASSERT_EQ(std::as_const(val).as<int>(), 0);
}
