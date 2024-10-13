#include <gtest/gtest.h>

#include <ez/Lambda.hpp>

#include <ez/Option.hpp>

using namespace ez;
using namespace ez::lambda::args;

TEST(Lambda, concept)
{
    static_assert(lambda::Lambda<decltype(arg1)>);
    static_assert(lambda::Lambda<decltype(arg1 + 1)>);
}

TEST(Lambda, UnaryPlus) { ASSERT_EQ((+arg1)(10), 10); }

TEST(Lambda, Deref) { ASSERT_EQ((*arg1)(Option{10}), 10); }

TEST(Lambda, Increment)
{
    int val = 10;
    auto f = ++arg1;
    f(val);
    ASSERT_EQ(val, 11);
}
TEST(Lambda, Decrement)
{
    int val = 10;
    auto f = --arg1;
    f(val);
    ASSERT_EQ(val, 9);
}

TEST(Lambda, PlusEq)
{
    int val = 10;
    auto f = arg1 += 10;
    f(val);
    ASSERT_EQ(val, 20);
}

TEST(Lambda, MinusEq)
{
    int val = 10;
    auto f = arg1 -= 10;
    f(val);
    ASSERT_EQ(val, 0);
}

TEST(Lambda, MulEq)
{
    int val = 10;
    auto f = arg1 *= 10;
    f(val);
    ASSERT_EQ(val, 100);
}

TEST(Lambda, DivEq)
{
    int val = 10;
    auto f = arg1 /= 2;
    f(val);
    ASSERT_EQ(val, 5);
}

TEST(Lambda, ModEq)
{
    int val = 10;
    auto f = arg1 %= 2;
    f(val);
    ASSERT_EQ(val, 0);
}

TEST(Lambda, BitAndEq)
{
    int val = 10;
    auto f = arg1 &= 2;
    f(val);
    ASSERT_EQ(val, 10 & 2);
}

TEST(Lambda, BitOrEq)
{
    int val = 10;
    auto f = arg1 |= 2;
    f(val);
    ASSERT_EQ(val, 10 | 2);
}

TEST(Lambda, BitXorEq)
{
    int val = 10;
    auto f = arg1 ^= 2;
    f(val);
    ASSERT_EQ(val, 10 ^ 2);
}

TEST(Lambda, LeftShiftEq)
{
    int val = 10;
    auto f = arg1 <<= 2;
    f(val);
    ASSERT_EQ(val, 10 << 2);
}

TEST(Lambda, RightShiftEq)
{
    int val = 10;
    auto f = arg1 >>= 2;
    f(val);
    ASSERT_EQ(val, 10 >> 2);
}

TEST(Lambda, Plus)
{
    ASSERT_EQ((arg1 + 10)(10), 20);
    ASSERT_EQ((arg1 + arg2)(10, 10), 20);
}

TEST(Lambda, Minus)
{
    ASSERT_EQ((arg1 - 10)(20), 10);
    ASSERT_EQ((arg1 - arg2)(20, 10), 10);
}

TEST(Lambda, Mul)
{
    ASSERT_EQ((arg1 * 10)(20), 200);
    ASSERT_EQ((arg1 * arg2)(20, 10), 200);
}

TEST(Lambda, Div)
{
    ASSERT_EQ((arg1 / 10)(20), 2);
    ASSERT_EQ((arg1 / arg2)(20, 10), 2);
}

TEST(Lambda, Mod)
{
    ASSERT_EQ((arg1 % 3)(10), 1);
    ASSERT_EQ((arg1 % arg2)(10, 3), 1);
}

TEST(Lambda, Negate) { ASSERT_EQ((-arg1)(10), -10); }

TEST(Lambda, Eq)
{
    ASSERT_TRUE((arg1 == 3)(3));
    ASSERT_TRUE((arg1 == arg2)(10, 10));
}

TEST(Lambda, NotEq)
{
    ASSERT_TRUE((arg1 != 3)(10));
    ASSERT_TRUE((arg1 != arg2)(10, 20));
}

TEST(Lambda, Greater)
{
    ASSERT_TRUE((arg1 > 3)(10));
    ASSERT_TRUE((arg1 > arg2)(20, 10));
}

TEST(Lambda, GreaterEq)
{
    ASSERT_TRUE((arg1 >= 3)(10));
    ASSERT_TRUE((arg1 >= arg2)(20, 20));
}

TEST(Lambda, Less)
{
    ASSERT_TRUE((arg1 < 3)(0));
    ASSERT_TRUE((arg1 < arg2)(2, 20));
}

TEST(Lambda, LessEq)
{
    ASSERT_TRUE((arg1 <= 20)(10));
    ASSERT_TRUE((arg1 <= arg2)(20, 20));
}

TEST(Lambda, And)
{
    ASSERT_TRUE((arg1 && true)(true));
    ASSERT_TRUE((arg1 && arg2)(true, true));
}

TEST(Lambda, Or)
{
    ASSERT_TRUE((arg1 || true)(false));
    ASSERT_TRUE((arg1 || arg2)(false, true));
}

TEST(Lambda, Not) { ASSERT_TRUE((!arg1)(false)); }

TEST(Lambda, BitAnd)
{
    ASSERT_EQ((arg1 & 1)(10), 10 & 1);
    ASSERT_EQ((arg1 & arg2)(10, 1), 10 & 1);
}

TEST(Lambda, BitOr)
{
    ASSERT_EQ((arg1 | 1)(10), 10 | 1);
    ASSERT_EQ((arg1 | arg2)(10, 1), 10 | 1);
}

TEST(Lambda, BitXor)
{
    ASSERT_EQ((arg1 ^ 1)(10), 10 ^ 1);
    ASSERT_EQ((arg1 ^ arg2)(10, 1), 10 ^ 1);
}

TEST(Lambda, BitNot)
{
    ASSERT_EQ((~arg1 )(10), ~10 );
}


