#pragma once

#include <ez/Enum.hpp>
#include <ez/Error.hpp>
#include <ez/ValueWrapper.hpp>

#include <memory>
#include <variant>

namespace ez {
template <typename T>
class Ok : public ValueWrapper<T> {
public:
    using Super = ValueWrapper<T>;
    using Super::Super;
    using Super::value;
};

template <>
class Ok<void> {
public:
};

template <typename T>
Ok(T&&) -> Ok<std::decay_t<T>>;
Ok() -> Ok<void>;

//////////////////////////////////////////////////////////////////////////////

template <typename E>
class Fail : public ValueWrapper<E> {
public:
    using Super = ValueWrapper<E>;
    using Super::Super;
    using Super::value;
};

template <typename T>
Fail(T&&) -> Fail<std::decay_t<T>>;

//////////////////////////////////////////////////////////////////////////////

///
/// Result<T, E> is the type used for returning and propagating errors. It is an enum with the
/// variants, Ok<T>, representing success and containing a value, and Fail<E>, representing error
/// and containing an error value. Usage:
/// @code
/// enum class Error { Cause1, Cause2 };
/// auto task = []() -> Result<std::string, Error> { return Ok{"hello"}; };
/// auto result = task();
/// ASSERT_TRUE(result);
/// ASSERT_EQ(result.value(), "hello");
///
/// auto task = []() -> Result<std::string, Error> { return Fail{Error::Cause1}; };
/// auto result = task();
/// ASSERT_FALSE(result);
/// ASSERT_TRUE(result.is_error());
/// ASSERT_EQ(result.error(), Error::Cause1);
///
/// @endcode
template <typename T, typename E = Error>
class [[nodiscard]] Result : public Enum<Ok<T>, Fail<E>> {
public:
    using Super = Enum<Ok<T>, Fail<E>>;

    using Super::Super;

    Result() = default;

    Result(Result&) = default;
    Result(const Result&) = default;
    Result(Result&& e) = default;

    Result& operator=(Result&) = default;
    Result& operator=(const Result&) = default;
    Result& operator=(Result&& e) = default;

    Result(const Ok<void>& val);
    Result(Ok<void>&& val);

    template <typename TT>
    Result(const Ok<TT>& val);

    template <typename TT>
    Result(Ok<TT>&& val);

    template <typename EE>
    Result(const Fail<EE>& e);

    template <typename EE>
    Result(Fail<EE>&& e);

    explicit operator bool() const;
    bool has_value() const;
    bool is_error() const;

    const T* operator->() const&;
    T* operator->() &;

    decltype(auto) value() const&;
    decltype(auto) value() &;
    decltype(auto) value() &&;

    const E& error() const&;
    E& error() &;
    E error() &&;

    template <typename V>
    T operator|(V&& val) const&;

    template <typename V>
    T operator|(V&& val) &&;

private:
    void has_value_or_throw() const
    {
        if (!has_value()) throw error();
    }
};

///////////////////////////////////////////////////////////////////////////////

template <typename T, typename E>
Result<T, E>::Result(const Ok<void>& val) : Super{val}
{
}

template <typename T, typename E>
Result<T, E>::Result(Ok<void>&& val) : Super{val}
{
}

template <typename T, typename E>
template <typename TT>
Result<T, E>::Result(const Ok<TT>& val) : Super{Ok<T>{val.value()}}
{
}

template <typename T, typename E>
template <typename TT>
Result<T, E>::Result(Ok<TT>&& val) : Super{Ok<T>{std::move(val).value()}}
{
}

template <typename T, typename E>
template <typename EE>
Result<T, E>::Result(const Fail<EE>& e) : Super{Fail<E>{e.value()}}
{
}

template <typename T, typename E>
template <typename EE>
Result<T, E>::Result(Fail<EE>&& e) : Super{Fail<E>{std::move(e).value()}}
{
}

template <typename T, typename E>
Result<T, E>::operator bool() const
{
    return Super::index() == 0;
}

template <typename T, typename E>
bool Result<T, E>::has_value() const
{
    return Super::index() == 0;
}

template <typename T, typename E>
bool Result<T, E>::is_error() const
{
    return Super::index() == 1;
}

template <typename T, typename E>
const T* Result<T, E>::operator->() const&
{
    return &value();
}

template <typename T, typename E>
T* Result<T, E>::operator->() &
{
    return &value();
}

template <typename T, typename E>
decltype(auto) Result<T, E>::value() const&
{
    has_value_or_throw();
    if constexpr (std::is_same_v<T, void>) { return; }
    else {
        return std::get<Ok<T>>(*this).value();
    }
}

template <typename T, typename E>
decltype(auto) Result<T, E>::value() &
{
    has_value_or_throw();
    if constexpr (std::is_same_v<T, void>) { return; }
    else {
        return std::get<Ok<T>>(*this).value();
    }
}

template <typename T, typename E>
decltype(auto) Result<T, E>::value() &&
{
    has_value_or_throw();
    if constexpr (std::is_same_v<T, void>) { return; }
    else {
        return std::move(std::get<Ok<T>>(*this)).value();
    }
}

template <typename T, typename E>
const E& Result<T, E>::error() const&
{
    return std::get<Fail<E>>(*this).value();
}

template <typename T, typename E>
E& Result<T, E>::error() &
{
    return std::get<Fail<E>>(*this).value();
}

template <typename T, typename E>
E Result<T, E>::error() &&
{
    return std::move(std::get<Fail<E>>(*this).value());
}

template <typename T, typename E>
template <typename V>
T Result<T, E>::operator|(V&& val) const&
{
    if (has_value()) return value();
    return std::forward<V>(val);
}

template <typename T, typename E>
template <typename V>
T Result<T, E>::operator|(V&& val) &&
{
    if (has_value()) return std::move(*this).value();
    return std::forward<V>(val);
}

///////////////////////////////////////////////////////////////////////////////

template <typename E = Error>
struct Try {
    template <typename F>
    Result<std::invoke_result_t<F>, E> operator<<(F&& f) const noexcept
    {
        using Ret = std::invoke_result_t<F>;

        try {
            if constexpr (std::is_void_v<Ret>) {
                std::forward<F>(f)();
                return Ok{};
            }
            else {
                return Ok{std::forward<F>(f)()};
            }
        }
        catch (E& error) {
            return Fail(std::move(error));
        }
    }
};

}  // namespace ez

#define EZ_ENSURE(res) \
    if (!res) return ez::Fail{std::move(res).error()};

#define EZ_CO_ENSURE(res) \
    if (!res) co_return ez::Fail{std::move(res).error()};

#define EZ_TRY ez::Try<>{} << [&]
