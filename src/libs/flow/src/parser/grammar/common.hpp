#pragma once

#include "../annotations.hpp"
#include "../ast_adapted.hpp"
#include "../x3_traits.hpp"

#include <ez/Preprocessor.hpp>

#include <boost/spirit/home/x3.hpp>

namespace ez::flow::grammar {

namespace x3 = boost::spirit::x3;
template <typename Id>
using RuleTag = parser::RuleTag<Id>;

#define EZ_FLOW_RULE0(name) const x3::rule<struct EZ_CONCAT(name, _tag)> name = #name
#define EZ_FLOW_RULE_IMPL(Tag, T, name) const x3::rule<RuleTag<Tag>, T> name = #name
#define EZ_FLOW_RULE(T, name) EZ_FLOW_RULE_IMPL(struct EZ_CONCAT(name, _tag), T, name)
#define EZ_FLOW_DEF(name) const auto EZ_CONCAT(name, _def)

///////////////////////////////////////////////////////////////////////////////

template <typename T>
struct Symbol : public x3::symbols<T> {
    void add_impl() {}

    void add_impl(auto name, auto value, auto... rest)
    {
        x3::symbols<T>::add(name, value);
        add_impl(rest...);
    }

    Symbol(auto... args) { add_impl(args...); }
};

///////////////////////////////////////////////////////////////////////////////

const auto space = x3::space;
const auto single_line_comment = "//" >> *(x3::char_ - x3::eol) >> (x3::eol | x3::eoi);

EZ_FLOW_RULE0(block_comment);
EZ_FLOW_DEF(block_comment) = "/*" >> *(block_comment | (x3::char_ - "*/")) >> "*/";
BOOST_SPIRIT_DEFINE(block_comment)

const auto comment = single_line_comment | block_comment;

const auto skipper = space | comment;

}  // namespace ez::flow::grammar
