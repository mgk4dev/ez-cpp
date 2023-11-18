#pragma once

#include "symbols.hpp"

namespace ez::flow::grammar {

EZ_FLOW_RULE(ast::SingleQuoteString, single_quote_string);
EZ_FLOW_RULE(ast::TripleQuoteString, triple_quote_string);
EZ_FLOW_RULE(ast::String, string);
EZ_FLOW_RULE(ast::Integer, integer);
EZ_FLOW_RULE(ast::Real, real);
EZ_FLOW_RULE(ast::Duration, duration);
EZ_FLOW_RULE(ast::Literal, literal);

EZ_FLOW_DEF(single_quote_string) = x3::lexeme['\'' >> +(x3::char_ - '\'') >> '\''];
EZ_FLOW_DEF(triple_quote_string) = x3::lexeme["'''" >> +(x3::char_ - "'''") >> "'''"];
EZ_FLOW_DEF(string) = single_quote_string | triple_quote_string;
EZ_FLOW_DEF(integer) = x3::int_;
EZ_FLOW_DEF(real) = x3::real_parser<double, x3::strict_real_policies<double>>{};

EZ_FLOW_DEF(duration) = x3::no_skip[x3::uint_ >> duration_unit];

// clang-format off
EZ_FLOW_DEF(literal) =
      triple_quote_string
    | single_quote_string
    | boolean
    | duration
    | real
    | integer;

BOOST_SPIRIT_DEFINE(single_quote_string,
                    triple_quote_string,
                    string,
                    integer,
                    real,
                    duration,
                    literal)

// clang-format on


}  // namespace ez::flow::grammar
