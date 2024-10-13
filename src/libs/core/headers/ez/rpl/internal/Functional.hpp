#pragma once

#include <ez/Tuple.hpp>

#include <boost/callable_traits/args.hpp>

#include <functional>

namespace ez::rpl::internal {

template <typename... Args>
struct PlaceholderCount {
    static const size_t value = 0;
};
template <typename T, typename... Args>
struct PlaceholderCount<T, Args...> {
    static const size_t value = PlaceholderCount<Args...>::value + !!std::is_placeholder<T>::value;
};

template <typename T, typename... Args>
struct BindArity;

template <typename T, typename... Args>
struct BindArity<T(Args...)> {
    static const size_t value = PlaceholderCount<Args...>::value;
};

template <template <typename, typename...> class X, typename T, typename... Args>
struct BindArity<X<T, Args...>> {
    static const size_t value = BindArity<T, Args...>::value;
};

template <typename F, typename Arg>
    requires(!std::is_bind_expression_v<std::decay_t<F>>)
decltype(auto) apply_fn(F&& f, Arg&& arg)
{
    using ConcreteArg = std::remove_cvref_t<Arg>;
    using ConcreteF = std::remove_cvref_t<F>;

    constexpr bool is_tuple = requires { tuple::get<0>(arg); };

    if constexpr (is_tuple) {
        using Args = boost::callable_traits::args_t<ConcreteF>;
        constexpr size_t arity = tuple::tuple_size_v<Args>;
        constexpr size_t tuple_size = tuple::tuple_size_v<ConcreteArg>;
        if constexpr (arity == tuple_size) { return tuple::apply(EZ_FWD(f), EZ_FWD(arg)); }
        else {
            return EZ_FWD(f)(EZ_FWD(arg));
        }
    }
    else {
        return EZ_FWD(f)(EZ_FWD(arg));
    }
}

template <typename F, typename Arg>
    requires(std::is_bind_expression_v<std::decay_t<F>>)
decltype(auto) apply_fn(F&& f, Arg&& arg)
{
    using ConcreteArg = std::remove_cvref_t<Arg>;
    using ConcreteF = std::remove_cvref_t<F>;

    constexpr bool is_tuple = requires { tuple::get<0>(arg); };

    if constexpr (is_tuple) {
        constexpr size_t arity = BindArity<ConcreteF>::value;
        constexpr size_t tuple_size = tuple::tuple_size_v<ConcreteArg>;
        if constexpr (arity == tuple_size) { return tuple::apply(EZ_FWD(f), EZ_FWD(arg)); }
        else {
            return EZ_FWD(f)(EZ_FWD(arg));
        }
    }
    else {
        return EZ_FWD(f)(EZ_FWD(arg));
    }
}

}  // namespace ez::rpl::internal
