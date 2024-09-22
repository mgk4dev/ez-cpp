#include <gtest/gtest.h>

#include <ez/Tuple.hpp>

using namespace ez;

TEST(Tupe, ctor)
{
    {
        Tuple tuple = std::make_tuple(1, 2, std::string{"hello"});
        static_assert(tuple.size() == 3);
    }

    {
        Tuple tuple{1, 2, "hello"};
        static_assert(tuple.size() == 3);
    }
}

TEST(Tupe, for_each)
{
    Tuple tuple{1, 2, 3};
    int result = 0;
    tuple.for_each([&](auto val) { result += val; });
    ASSERT_EQ(result, 6);
}

TEST(Tupe, transformed)
{
    Tuple tuple{1, 2, 3};

    auto strings = tuple.transformed([](auto val) { return std::to_string(val); });

    static_assert(
        std::is_same_v<EZ_DECAY_T(strings), Tuple<std::string, std::string, std::string> >);

    ASSERT_EQ(strings[EZ_CONSTEXP(0)], "1");
    ASSERT_EQ(strings[EZ_CONSTEXP(1)], "2");
    ASSERT_EQ(strings[EZ_CONSTEXP(2)], "3");
}

TEST(Tuple, structured_bindings)
{
    Tuple tuple{1, std::string{"hello"}};

    auto& [v1, v2] = tuple;

    v1 = 10;
    v2 = "toto";

    ASSERT_EQ(tuple[EZ_CONSTEXP(0)], 10);
    ASSERT_EQ(tuple[EZ_CONSTEXP(1)], "toto");
}
