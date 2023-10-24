#include <gtest/gtest.h>

#include <ez/Utils.hpp>

using namespace ez;

TEST(Utils, constexpr)
{
    auto is_2 = [](Constexpr<int> auto val) -> int {
        static_assert(val == 2);
        return val;
    };

    ASSERT_EQ(is_2(ct_<2>), 2);
}
