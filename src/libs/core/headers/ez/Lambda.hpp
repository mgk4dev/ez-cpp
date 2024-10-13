#pragma once

#include <ez/Tuple.hpp>

#include <functional>

namespace ez::lambda {

template <size_t index>
struct Arg {
    constexpr decltype(auto) operator()(auto&&... args) const noexcept
    {
        return tuple::arg_at<index - 1>(EZ_FWD(args)...);
    }

    constexpr auto operator[](auto&& t) const
    {
        return std::bind([](auto&& t1, auto&& t2) { return EZ_FWD(t1)[EZ_FWD(t2)]; }, *this,
                         EZ_FWD(t));
    }
};

namespace args {
inline constexpr Arg<1> arg1{};
inline constexpr Arg<2> arg2{};
inline constexpr Arg<3> arg3{};
}  // namespace args

// ----------------------------------------------------------------------------

template <typename T>
concept Lambda = std::is_placeholder_v<std::remove_cvref_t<T>> > 0 ||
                 std::is_bind_expression_v<std::remove_cvref_t<T>>;

// ----------------------------------------------------------------------------

#define EZ_LAMBDA_UNARY_FN(op, Fn)                                             \
    struct Fn {                                                                \
        decltype(auto) operator()(auto&& val) const { return op EZ_FWD(val); } \
    };

#define EZ_LAMBDA_POSTFIX_FN(op, Fn)                                           \
    struct Fn {                                                                \
        decltype(auto) operator()(auto&& val) const { return EZ_FWD(val) op; } \
    };

#define EZ_LAMBDA_BINARY_FN(op, Fn)                                                            \
    struct Fn {                                                                                \
        decltype(auto) operator()(auto&& a, auto&& b) const { return EZ_FWD(a) op EZ_FWD(b); } \
    };

// ----------------------------------------------------------------------------

#define EZ_UNARY_LAMBDA(op, Fn) \
    auto operator op(Lambda auto&& a) { return std::bind(Fn(), EZ_FWD(a)); }

#define EZ_POSTFIX_LAMBDA(op, Fn) \
    auto operator op(Lambda auto&& a, int) { return std::bind(Fn(), EZ_FWD(a)); }

#define EZ_BINARY_LAMBDA(op, Fn)                                        \
    template <typename T, typename U>                                   \
        requires Lambda<T> || Lambda<U>                                 \
    auto operator op(T&& a, U&& b)                                      \
    {                                                                   \
        return std::bind(Fn(), std::forward<T>(a), std::forward<U>(b)); \
    }

// ----------------------------------------------------------------------------

EZ_LAMBDA_UNARY_FN(+, UnaryPlus)
EZ_LAMBDA_UNARY_FN(*, Deref)

EZ_LAMBDA_UNARY_FN(++, Increment)
EZ_LAMBDA_UNARY_FN(--, Decrement)

EZ_LAMBDA_POSTFIX_FN(++, PostfixIncrement)
EZ_LAMBDA_POSTFIX_FN(--, PostfixDecrement)

EZ_LAMBDA_BINARY_FN(+=, PlusEq)
EZ_LAMBDA_BINARY_FN(-=, MinusEq)
EZ_LAMBDA_BINARY_FN(*=, MulEq)
EZ_LAMBDA_BINARY_FN(/=, DivEq)
EZ_LAMBDA_BINARY_FN(%=, ModEq)
EZ_LAMBDA_BINARY_FN(&=, BitAndEq)
EZ_LAMBDA_BINARY_FN(|=, BitOrEq)
EZ_LAMBDA_BINARY_FN(^=, BitXorEq)
EZ_LAMBDA_BINARY_FN(<<=, LeftShiftEq)
EZ_LAMBDA_BINARY_FN(>>=, RightShiftEq)

// --------------------------------------------------------

EZ_BINARY_LAMBDA(+, std::plus<>)
EZ_BINARY_LAMBDA(-, std::minus<>)
EZ_BINARY_LAMBDA(*, std::multiplies<>)
EZ_BINARY_LAMBDA(/, std::divides<>)
EZ_BINARY_LAMBDA(%, std::modulus<>)
EZ_UNARY_LAMBDA(-, std::negate<>)

EZ_BINARY_LAMBDA(==, std::equal_to<>)
EZ_BINARY_LAMBDA(!=, std::not_equal_to<>)
EZ_BINARY_LAMBDA(>, std::greater<>)
EZ_BINARY_LAMBDA(<, std::less<>)
EZ_BINARY_LAMBDA(>=, std::greater_equal<>)
EZ_BINARY_LAMBDA(<=, std::less_equal<>)

EZ_BINARY_LAMBDA(&&, std::logical_and<>)
EZ_BINARY_LAMBDA(||, std::logical_or<>)
EZ_UNARY_LAMBDA(!, std::logical_not<>)

EZ_BINARY_LAMBDA(&, std::bit_and<>)
EZ_BINARY_LAMBDA(|, std::bit_or<>)
EZ_BINARY_LAMBDA(^, std::bit_xor<>)
EZ_UNARY_LAMBDA(~, std::bit_not<>)

EZ_UNARY_LAMBDA(+, UnaryPlus)
EZ_UNARY_LAMBDA(*, Deref)

EZ_UNARY_LAMBDA(++, Increment)
EZ_UNARY_LAMBDA(--, Decrement)

EZ_POSTFIX_LAMBDA(++, PostfixIncrement)
EZ_POSTFIX_LAMBDA(--, PostfixDecrement)

EZ_BINARY_LAMBDA(+=, PlusEq)
EZ_BINARY_LAMBDA(-=, MinusEq)
EZ_BINARY_LAMBDA(*=, MulEq)
EZ_BINARY_LAMBDA(/=, DivEq)
EZ_BINARY_LAMBDA(%=, ModEq)
EZ_BINARY_LAMBDA(&=, BitAndEq)
EZ_BINARY_LAMBDA(|=, BitOrEq)
EZ_BINARY_LAMBDA(^=, BitXorEq)
EZ_BINARY_LAMBDA(<<=, LeftShiftEq)
EZ_BINARY_LAMBDA(>>=, RightShiftEq)

}  // namespace ez::lambda

namespace std {
template <size_t I>
struct is_placeholder<ez::lambda::Arg<I>> : integral_constant<int, I> {};
}  // namespace std
