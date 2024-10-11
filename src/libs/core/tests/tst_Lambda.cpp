#include <gtest/gtest.h>

#include <ez/Lambda.hpp>

using namespace ez;

TEST(Lambda, 1)
{
    using namespace ez::lambda::args;

    auto f = (arg1 > 10);

    bool ok = f(20);
    ASSERT_TRUE(ok);
}

TEST(Lambda, 2)
{
    using namespace ez::lambda::args;

    auto f = (arg1 == arg2 || arg1 == arg3);

    bool ok = f(1, 2, 1);
    ASSERT_TRUE(ok);
}
