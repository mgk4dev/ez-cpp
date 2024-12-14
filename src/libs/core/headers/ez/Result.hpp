#pragma once

#include <ez/Error.hpp>

#include <expected>

namespace ez {

template <typename E>
using Fail = std::unexpected<E>;

struct Ok {};


///
/// Result<T, E> is the type used for returning and propagating errors. It is an enum with the
/// variants, T representing success and containing a value, and E, representing error
/// and containing an error value. Usage:
/// @code
/// enum class Error { Cause1, Cause2 };
/// auto task = []() -> Result<std::string, Error> { return "hello"; };
/// auto result = task();
/// ASSERT_TRUE(result);
/// ASSERT_EQ(result.value(), "hello");
///
/// auto task = []() -> Result<std::string, Error> { return Fail{Error::Cause1}; };
/// auto result = task();
/// ASSERT_FALSE(result);
/// ASSERT_EQ(result.error(), Error::Cause1);
///
/// @endcode
template <typename T, typename E = Error>
class Result : public std::expected<T, E> {
public:
    using Super = std::expected<T, E>;

    using Super::Super;

    Result(Ok)
        requires std::is_same_v<T, void>
        : Super{}
    {
    }

    template <typename EE>
    Result(const Fail<EE>& error) : Super{Fail<E>(error.error())}
    {
    }

    template <typename EE>
    Result(Fail<EE>&& error) : Super{Fail<E>(std::move(error).error())}
    {
    }
};

}  // namespace ez
