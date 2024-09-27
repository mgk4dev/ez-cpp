#pragma once

#include "common.hpp"

#include <array>
#include <iostream>

namespace ez::flow::grammar {
const auto semicolon = x3::lexeme[';'];
const auto semicolons = +semicolon;
const auto optional_semicolons = *semicolon;

// clang-format off

const Symbol<bool> boolean{"true", true, "false", false};

const Symbol<ast::DurationUnit> duration_unit{
    "ms",    ast::DurationUnit::Ms,
    "sec",   ast::DurationUnit::Sec,
    "mn",    ast::DurationUnit::Min,
    "day",   ast::DurationUnit::Day,
    "week",  ast::DurationUnit::Week};

const Symbol<ast::LogicalOperator> logical_op{
    "and", ast::LogicalOperator::And,
    "or",  ast::LogicalOperator::Or
};

const Symbol<ast::EqualityOperator> equality_op{
    "==", ast::EqualityOperator::Eq,
    "!=", ast::EqualityOperator::NotEq,
    "in", ast::EqualityOperator::In,
    "is", ast::EqualityOperator::Is
};

const Symbol<ast::RelationalOperator> relational_op{
    "<", ast::RelationalOperator::Less,
    "<=", ast::RelationalOperator::LessEq,
    ">", ast::RelationalOperator::Greater,
    ">=", ast::RelationalOperator::GreaterEq
};

const Symbol<ast::AdditiveOperator> additive_op{
    "+", ast::AdditiveOperator::Plus,
    "-", ast::AdditiveOperator::Minus
};

const Symbol<ast::MultiplicativeOperator> multiplicative_op{
    "*", ast::MultiplicativeOperator::Mult,
    "/", ast::MultiplicativeOperator::Div
};

const Symbol<ast::UnaryOperator> unary_op{
    "+",   ast::UnaryOperator::Plus,
    "-",   ast::UnaryOperator::Minus,
    "not", ast::UnaryOperator::Not,
};

struct Keywords : x3::symbols<> {
    Keywords()
    {
        static const std::array kws{"import", "module", "delay", "return", "raise", "break",
                                    "try",    "repeat", "if",    "await",  "try",    "elif",
                                    "else",   "in",     "is",    "for",    "and",    "or",   "not"};
        for (const auto kw : kws) add(kw);
    }
} const keyword;

struct BuiltinTypes : x3::symbols<> {
    BuiltinTypes()
    {
        static const std::array types{
                                      "workflow",    "workflow_reply",
                                      "campaign",    "campaign_reply",
                                      "action",      "action_reply",
                                      "http_request","http_reply",
                                      "device_info_request", "device_info_reply"};
        for (const auto type : types) add(type);
    }
} const builtin_types;

// clang-format on

/////////////////////////////////////////////////////////////////////////

EZ_FLOW_RULE(ast::Identifier, identifier);
EZ_FLOW_RULE(ast::IdentifierPath, identifier_path);

const auto identifier_base = x3::raw[x3::lexeme[(x3::alpha | '_') >> *(x3::alnum | '_')]];

auto reserved = x3::lexeme[keyword >> !identifier_base];

EZ_FLOW_DEF(identifier) = x3::lexeme[identifier_base] - reserved;
EZ_FLOW_DEF(identifier_path) = identifier >> *('.' >> identifier);
BOOST_SPIRIT_DEFINE(identifier, identifier_path)

}  // namespace ez::flow::grammar
