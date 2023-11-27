#pragma once

#include "literals.hpp"
#include "symbols.hpp"

namespace ez::flow::grammar {

EZ_FLOW_RULE(ast::Declaration, declaration);
EZ_FLOW_RULE(ast::DeclarationList, declaration_list);
EZ_FLOW_RULE(ast::Signature, signature);
EZ_FLOW_RULE(ast::Assignment, assignment);
EZ_FLOW_RULE(ast::Dictionary, dictionary);
EZ_FLOW_RULE(ast::Array, array);

EZ_FLOW_RULE(ast::FunctionArgument, function_argument);
EZ_FLOW_RULE(ast::FunctionCall, function_call);

EZ_FLOW_RULE(ast::PrimaryExpression, primary_expr);

EZ_FLOW_RULE(ast::MultiplicativeExpression, multiplicative_expr);
EZ_FLOW_RULE(ast::AdditiveExpression, additive_expr);
EZ_FLOW_RULE(ast::RelationalExpression, relational_expr);
EZ_FLOW_RULE(ast::EqualityExpression, equality_expr);
EZ_FLOW_RULE(ast::UnaryExpression, unary_expr);
EZ_FLOW_RULE(ast::LogicalExpression, logical_expr);
EZ_FLOW_RULE(ast::Expression, expression);

EZ_FLOW_RULE(ast::AwaitExpression, await_expression);
EZ_FLOW_RULE(ast::TryExpression, try_expression);

///////////////////////////////////////

EZ_FLOW_DEF(declaration) = identifier >> ':' >> identifier;

EZ_FLOW_DEF(declaration_list) = (declaration % ',');

EZ_FLOW_DEF(signature) = '(' >> -declaration_list >> ')' >> -("->" > identifier);

EZ_FLOW_DEF(assignment) = identifier >> '=' >> expression;

EZ_FLOW_DEF(dictionary) = '(' >> -(assignment % ',') >> ')';
EZ_FLOW_DEF(array) = '[' >> -(expression % ',') >> ']';

EZ_FLOW_DEF(function_argument) = -(identifier >> '=') >> expression;

EZ_FLOW_DEF(function_call) = identifier_path >> '(' >> -(function_argument % ',') >> ')';

//////////////////////////////////////

// clang-format off
EZ_FLOW_DEF(primary_expr) =
      await_expression
    | try_expression
    | literal
    | function_call
    | identifier_path
    | array
    | dictionary
    | ('(' > expression > ')')
    ;
// clang-format on

EZ_FLOW_DEF(await_expression) = "await" >> expression >> "for" >> expression;

EZ_FLOW_DEF(try_expression) = "try" >> expression;

EZ_FLOW_DEF(unary_expr) = -unary_op >> primary_expr;

EZ_FLOW_DEF(multiplicative_expr) = unary_expr >> *(multiplicative_op > unary_expr);

EZ_FLOW_DEF(additive_expr) = multiplicative_expr >> *(additive_op > multiplicative_expr);

EZ_FLOW_DEF(relational_expr) = additive_expr >> *(relational_op > additive_expr);

EZ_FLOW_DEF(equality_expr) = relational_expr >> *(equality_op > relational_expr);

EZ_FLOW_DEF(logical_expr) = equality_expr >> *(logical_op > equality_expr);

EZ_FLOW_DEF(expression) = logical_expr;

///////////////////////////////////////

BOOST_SPIRIT_DEFINE(declaration, declaration_list, signature, assignment)

BOOST_SPIRIT_DEFINE(function_call,
                    function_argument,
                    dictionary,
                    array,
                    equality_expr,
                    relational_expr,
                    additive_expr,
                    multiplicative_expr,
                    unary_expr,
                    primary_expr,
                    logical_expr,
                    expression,
                    await_expression,
                    try_expression)

}  // namespace ez::flow::grammar
