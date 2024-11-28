#pragma once

#include <ez/OneOf.hpp>

#include <boost/spirit/home/x3/support/traits/is_variant.hpp>
#include <boost/spirit/home/x3/support/traits/variant_find_substitute.hpp>
#include <boost/spirit/home/x3/support/traits/variant_has_substitute.hpp>

namespace boost::spirit::x3::traits {
template <typename... Ts>
struct is_variant<ez::OneOf<Ts...>> : mpl::true_ {};

template <typename attribute, typename... Ts>
struct variant_has_substitute_impl<ez::OneOf<Ts...>, attribute> {
    typedef ez::OneOf<Ts...> variant_type;
    typedef typename mpl::transform<mpl::list<Ts...>, unwrap_recursive<mpl::_1>>::type types;
    typedef typename mpl::end<types>::type end;

    typedef typename mpl::find<types, attribute>::type iter_1;

    typedef typename mpl::eval_if<is_same<iter_1, end>,
                                  mpl::find_if<types, traits::is_substitute<mpl::_1, attribute>>,
                                  mpl::identity<iter_1>>::type iter;

    typedef mpl::not_<is_same<iter, end>> type;
};

template <typename attribute, typename... Ts>
struct variant_find_substitute<ez::OneOf<Ts...>, attribute> {
    typedef ez::OneOf<Ts...> variant_type;
    typedef typename mpl::transform<mpl::list<Ts...>, unwrap_recursive<mpl::_1>>::type types;

    typedef typename mpl::end<types>::type end;

    typedef typename mpl::find<types, attribute>::type iter_1;

    typedef typename mpl::eval_if<is_same<iter_1, end>,
                                  mpl::find_if<types, traits::is_substitute<mpl::_1, attribute>>,
                                  mpl::identity<iter_1>>::type iter;

    typedef
        typename mpl::eval_if<is_same<iter, end>, mpl::identity<attribute>, mpl::deref<iter>>::type
            type;
};

template <typename... Ts>
struct variant_find_substitute<ez::OneOf<Ts...>, ez::OneOf<Ts...>>
    : mpl::identity<ez::OneOf<Ts...>> {};
}  // namespace boost::spirit::x3::traits
