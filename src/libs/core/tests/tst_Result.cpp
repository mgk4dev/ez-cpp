#include <gtest/gtest.h>

#include <ez/Result.hpp>

using namespace ez;

enum class ErrorCode { Cause1, Cause2 };

TEST(Result, valid_by_default)
{
    Result<std::string, ErrorCode> result;

    ASSERT_TRUE(result.has_value());
    ASSERT_TRUE(result);
    ASSERT_EQ(result.value(), "");
}

TEST(Result, void_value_is_accepted)
{
    Result<void, ErrorCode> result;

    ASSERT_TRUE(result.has_value());
    ASSERT_TRUE(result);
}

TEST(Result, valid_by_default_with_value)
{
    auto task = []() -> Result<std::string, ErrorCode> { return Ok{"hello"}; };

    auto result = task();

    ASSERT_TRUE(result.has_value());

    ASSERT_TRUE(result);
    ASSERT_EQ(result.value(), "hello");
}

TEST(Result, created_properly_from_fail_type)
{
    auto task = []() -> Result<std::string, ErrorCode> { return Fail{ErrorCode::Cause1}; };
    auto result = task();
    ASSERT_TRUE(!result);
    ASSERT_TRUE(result.is_error());
    ASSERT_EQ(result.error(), ErrorCode::Cause1);
}

TEST(Result, access_value_from_error_throws)
{
    auto task = []() -> Result<std::string, ErrorCode> { return Fail{ErrorCode::Cause1}; };

    auto result = task();

    ASSERT_THROW(result.value(), ErrorCode);
}

TEST(Result, is_copy_contructible_from_result)
{
    Result<std::string, ErrorCode> result = Ok{"hello"};
    Result<std::string, ErrorCode> copy = result;

    ASSERT_TRUE(copy);
    ASSERT_EQ(copy.value(), result.value());
}

TEST(Result, is_copyable_from_result)
{
    Result<std::string, ErrorCode> result = Ok{"hello"};
    Result<std::string, ErrorCode> copy;
    copy = result;

    ASSERT_TRUE(copy);
    ASSERT_EQ(copy.value(), result.value());
}

TEST(Result, is_copy_constructible_from_error)
{
    Result<std::string, ErrorCode> resultError = Fail{ErrorCode::Cause1};
    auto errorCopy = resultError;

    ASSERT_FALSE(resultError);
    ASSERT_EQ(resultError.error(), errorCopy.error());
}

TEST(Result, is_copyable_from_erro)
{
    Result<std::string, ErrorCode> result = Fail{ErrorCode::Cause1};
    Result<std::string, ErrorCode> copy;
    copy = result;

    ASSERT_FALSE(result);
    ASSERT_EQ(result.error(), copy.error());
}

TEST(Result, is_move_constructible)
{
    std::vector data{1, 2, 3, 4};
    const auto* data_ptr = data.data();

    Result<std::vector<int>, ErrorCode> result = Ok{std::move(data)};
    const auto* ptr = result.value().data();
    ASSERT_EQ(data_ptr, ptr);

    Result<std::vector<int>, ErrorCode> copy = std::move(result);
    ASSERT_TRUE(copy);
    ASSERT_EQ(copy.value().data(), data_ptr);
}

TEST(Result, is_move_constructible_from_error)
{
    std::vector error{1, 2, 3};
    const auto* data = error.data();

    Result<int, std::vector<int>> result = Fail{std::move(error)};
    ASSERT_FALSE(result);

    ASSERT_EQ(result.error().data(), data);
}

TEST(Result, value_and_error_can_have_the_same_type)
{
    // happy path
    {
        Result<std::string, std::string> result = Ok{"success"};
        ASSERT_TRUE(result);
    }

    // error path
    {
        Result<std::string, std::string> result = Fail{"error"};
        ASSERT_FALSE(result);
    }
}

TEST(Result, value_or)
{
    // happy path
    {
        Result<int, std::string> result = Ok{55};
        ASSERT_EQ(result or 5, 55);
    }

    // error path
    {
        Result<int, std::string> result = Fail{"error"};
        ASSERT_EQ(result or 5, 5);
    }
}
