#pragma once

#include <ez/enum.hpp>
#include <ez/value_wrapper.hpp>

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
Ok()->Ok<void>;

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

template <typename T, typename E>
class Result : public Enum<Ok<T>, Fail<E>> {
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

    Result(const Ok<void>& val) : Super{val} {}
    Result(Ok<void>&& val) : Super{val} {}

    template <typename TT>
    Result(const Ok<TT>& val) : Super{Ok<T>{val.value()}}
    {
    }

    template <typename TT>
    Result(Ok<TT>&& val) : Super{Ok<T>{std::move(val).value()}}
    {
    }

    template <typename EE>
    Result(const Fail<EE>& e) : Super{Fail<E>{e.value()}}
    {
    }

    template <typename EE>
    Result(Fail<EE>&& e) : Super{Fail<E>{std::move(e).value()}}
    {
    }

    explicit operator bool() const { return Super::index() == 0; }
    bool has_value() const { return Super::index() == 0; }
    bool is_error() const { return Super::index() == 1; }

    decltype(auto) value() const&
    {
        has_value_or_throw();
        if constexpr (std::is_same_v<T, void>) { return; }
        else {
            return std::get<Ok<T>>(*this).value();
        }
    }

    decltype(auto) value() &
    {
        has_value_or_throw();
        if constexpr (std::is_same_v<T, void>) { return; }
        else {
            return std::get<Ok<T>>(*this).value();
        }
    }
    decltype(auto) value() &&
    {
        has_value_or_throw();
        if constexpr (std::is_same_v<T, void>) { return; }
        else {
            return std::move(std::get<Ok<T>>(*this)).value();
        }
    }

    const E& error() const& { return std::get<Fail<E>>(*this).value(); }
    E& error() & { return std::get<Fail<E>>(*this).value(); }
    E error() && { return std::move(std::get<Fail<E>>(*this).value()); }

    const Fail<E>& wrapped_error() const& { return std::get<Fail<E>>(*this); }
    Fail<E>& wrapped_error() & { return std::get<Fail<E>>(*this); }
    Fail<E> wrapped_error() && { return std::move(std::get<Fail<E>>(*this)); }

    T operator|(auto&& val) const&
    {
        if (has_value()) return value();
        return EZ_FWD(val);
    }

    T operator|(auto&& val) &&
    {
        if (has_value()) return std::move(*this).value();
        return EZ_FWD(val);
    }

private:
    void has_value_or_throw() const
    {
        if (!has_value()) throw error();
    }
};

}  // namespace ez
