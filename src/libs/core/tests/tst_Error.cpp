#include <gtest/gtest.h>

#include <ez/Error.hpp>
#include <ez/Result.hpp>

namespace mylib {
enum class FooError { Reason1, Reason2 };

enum class UnusedError { Reason1, Reason2 };

inline std::string to_string(FooError value) noexcept
{
    switch (value) {
        case FooError::Reason1: return "Reason 1";
        case FooError::Reason2: return "Reason 2";
    }
    return "Success";
}

inline std::string to_string(UnusedError) noexcept { return "Success"; }

}  // namespace mylib

EZ_ERROR_INFO(mylib::FooError, "FOO-ERROR", ez::fn_overload<mylib::FooError>(mylib::to_string))
EZ_ERROR_INFO(mylib::UnusedError, "UNUSED-ERROR", ez::fn_overload<mylib::UnusedError>(mylib::to_string))

using namespace ez;
using namespace mylib;

TEST(Error, construction)
{
    Error error = FooError::Reason1;
    ASSERT_EQ(error.code(), as_int(FooError::Reason1));
    ASSERT_EQ(error.name(), "FOO-ERROR");
    ASSERT_TRUE(error.is<FooError>());
}

TEST(Error, comparison)
{
    Error error = FooError::Reason1;
    Error error2 = FooError::Reason1;

    ASSERT_EQ(error, FooError::Reason1);
    ASSERT_NE(error, FooError::Reason2);

    ASSERT_NE(error, UnusedError::Reason1);
    ASSERT_NE(error, UnusedError::Reason2);

    ASSERT_EQ(error, error2);
}

TEST(Error, id)
{
    Error error = FooError::Reason1;

    ASSERT_EQ(error.id(), "FOO-ERROR-0");
}

TEST(Error, result)
{
    auto f = []() -> Result<std::string> { return Fail(FooError::Reason1); };

    auto result = f();
    ASSERT_FALSE(result);
    ASSERT_EQ(result.error(), FooError::Reason1);
}

TEST(Error, result_throw_error_code)
{
    auto f = []() -> Result<std::string> { return Fail(FooError::Reason1); };

    auto result = f();
    try {
        std::cout << result.value();
        FAIL();
    }
    catch (const Error& code) {
        ASSERT_EQ(code, FooError::Reason1);
    }
}

TEST(Error, result_throw_exception)
{
    auto f = []() -> Result<std::string> { return Fail(FooError::Reason1); };

    auto result = f();
    try {
        std::cout << result.value();
        FAIL();
    }
    catch (const std::exception& exception) {
        ASSERT_EQ(std::string(exception.what()), "Reason 1");
    }
}
