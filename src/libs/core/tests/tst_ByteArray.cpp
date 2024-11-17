#include <gtest/gtest.h>

#include <ez/ByteArray.hpp>

using namespace ez;

TEST(ByteArray, to_string)
{
    ByteArray bytes = ByteArray::from_pod_bytes<std::uint32_t>(0xabcd);
    const auto str = bytes.to_hex_string();
    auto bytes_from_string = ByteArray::from_hex_string(str);

    ASSERT_TRUE(bytes_from_string);
    ASSERT_EQ(bytes, *bytes_from_string);
    ASSERT_EQ(bytes.size(), sizeof(std::uint32_t));
}

TEST(ByteArray, string)
{
    {
        const std::string str = "0123456789abcdef";
        ASSERT_TRUE(ByteArray::from_hex_string(str));
        ASSERT_EQ(str, ByteArray::from_hex_string(str)->to_hex_string());
    }
    {
        const std::string str = "azertyuiopqsdfgh";
        ASSERT_FALSE(ByteArray::from_hex_string(str));
    }
}

TEST(ByteArray, pod)
{
    {
        using T = std::uint64_t;
        T val = 0x0123456789abcdef;
        ASSERT_EQ(val, ByteArray::from_pod_bytes(val).to_pod_bytes<T>());
        ASSERT_EQ("0123456789abcdef", ByteArray::from_pod_bytes(val).to_hex_string());
    }
    {
        using T = std::uint8_t;
        T val = 0b00000001;
        ASSERT_EQ(val, ByteArray::from_pod_bytes(val).to_pod_bytes<T>());
        ASSERT_EQ("01", ByteArray::from_pod_bytes(val).to_hex_string());
    }

}
