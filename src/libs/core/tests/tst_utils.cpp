#include <gtest/gtest.h>

#include <ez/utils.hpp>

using namespace ez;

TEST(Utils, constexpr)
{
    auto is_2 = [](Constexpr<int> auto val) -> int {
        static_assert(val == 2);
        return val;
    };

    ASSERT_EQ(is_2(EZ_CONSTEXP(2)), 2);
}
