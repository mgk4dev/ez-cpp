#include <gtest/gtest.h>

#include <ez/rpc/serializer.hpp>

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

TEST(Rpc, serializer_request)
{
    test_serializer(Request{RequestId{"id"}, "", "foo",
                            std::vector<ByteArray>{ByteArray{"45"}, ByteArray{"32"}}});
}

TEST(Rpc, serializer_reply)
{
    test_serializer(Reply{RequestId{"id"}, ByteArray{"45"}, ReplyType::Value});
}

TEST(Rpc, serializer_error) { test_serializer(Error::remote_error("Function not found")); }
