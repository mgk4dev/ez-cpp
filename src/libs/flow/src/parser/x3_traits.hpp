#pragma once

#include <ez/Enum.hpp>

#include <boost/spirit/home/x3/support/traits/is_variant.hpp>
#include <boost/spirit/home/x3/support/traits/variant_find_substitute.hpp>
#include <boost/spirit/home/x3/support/traits/variant_has_substitute.hpp>

namespace boost::spirit::x3::traits {
template <typename... Ts>
struct is_variant<ez::Enum<Ts...>> : mpl::true_ {};

template <typename attribute, typename... Ts>
struct variant_has_substitute_impl<ez::Enum<Ts...>, attribute> {
    typedef ez::Enum<Ts...> variant_type;
    typedef typename mpl::transform<mpl::list<Ts...>, unwrap_recursive<mpl::_1>>::type types;
    typedef typename mpl::end<types>::type end;

    typedef typename mpl::find<types, attribute>::type iter_1;

    typedef typename mpl::eval_if<is_same<iter_1, end>,
                                  mpl::find_if<types, traits::is_substitute<mpl::_1, attribute>>,
                                  mpl::identity<iter_1>>::type iter;

    typedef mpl::not_<is_same<iter, end>> type;
};

template <typename attribute, typename... Ts>
struct variant_find_substitute<ez::Enum<Ts...>, attribute> {
    typedef ez::Enum<Ts...> variant_type;
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
struct variant_find_substitute<ez::Enum<Ts...>, ez::Enum<Ts...>> : mpl::identity<ez::Enum<Ts...>> {
};
}  // namespace boost::spirit::x3::traits
