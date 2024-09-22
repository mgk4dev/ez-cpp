#include <gtest/gtest.h>

#include <ez/rpc/Serializer.hpp>

using namespace ez::rpc;

template <typename T>
void test_serializer(T input)
{
    auto data = serialize(input);
    auto result = deserialize<T>(data);

    ASSERT_TRUE(result) << result.error().what();
    ASSERT_EQ(result.value(), input);
}

TEST(Rpc, serializer_int) { test_serializer(45); }
