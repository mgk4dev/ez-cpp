#include <gtest/gtest.h>

#include <ez/enum.hpp>
#include <ez/lambda.hpp>

using namespace ez;

TEST(Enum, case)
{
    auto f = Case<int> = Return{1};

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

TEST(Enum, match)
{
    Enum<int, double, std::string, std::vector<int>> val = "hello";

    // clang-format off
    auto index = val.match(
        Case<int> = Return{1},
        Case<> = Return{4}
    );
    // clang-format on

    ASSERT_EQ(index, 4);
}
