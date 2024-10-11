#pragma once

#include <ez/Tuple.hpp>

#include <boost/callable_traits/args.hpp>

namespace ez::rpl::internal {

template <typename F, typename Arg>
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
            return std::invoke(EZ_FWD(f), EZ_FWD(arg));
        }
    }
    else {
        return std::invoke(EZ_FWD(f), EZ_FWD(arg));
    }
}

}  // namespace ez::rpl::internal
