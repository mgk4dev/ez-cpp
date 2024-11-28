#pragma once

#include <ez/flow/Ast.hpp>

#include <boost/fusion/include/adapt_struct.hpp>

BOOST_FUSION_ADAPT_TPL_STRUCT((T)(Tag), (ez::flow::ast::PodElement)(T)(Tag), (T, value))

BOOST_FUSION_ADAPT_STRUCT(ez::flow::ast::Duration, count, unit);

BOOST_FUSION_ADAPT_STRUCT(ez::flow::ast::Declaration, name, type);
BOOST_FUSION_ADAPT_STRUCT(ez::flow::ast::Signature, inputs, return_type);

BOOST_FUSION_ADAPT_STRUCT(ez::flow::ast::FunctionCall, path, arguments);

BOOST_FUSION_ADAPT_STRUCT(ez::flow::ast::ImportStatement, path);
BOOST_FUSION_ADAPT_STRUCT(ez::flow::ast::DelayStatement, expression);
BOOST_FUSION_ADAPT_STRUCT(ez::flow::ast::ReturnStatement, expression);
BOOST_FUSION_ADAPT_STRUCT(ez::flow::ast::RaiseStatement, expression);

BOOST_FUSION_ADAPT_STRUCT(ez::flow::ast::FunctionArgument, name, value);
BOOST_FUSION_ADAPT_STRUCT(ez::flow::ast::Assignment, name, value);

BOOST_FUSION_ADAPT_STRUCT(ez::flow::ast::UnaryExpression, op, expression);

BOOST_FUSION_ADAPT_TPL_STRUCT((T)(Operator),
                              (ez::flow::ast::Operation)(T)(Operator),
                              (Operator, op),
                              (T, term))

BOOST_FUSION_ADAPT_TPL_STRUCT((T)(Operator),
                              (ez::flow::ast::BinaryExpression)(T)(Operator),
                              first,
                              terms)

BOOST_FUSION_ADAPT_STRUCT(ez::flow::ast::Expression, first, terms);
BOOST_FUSION_ADAPT_STRUCT(ez::flow::ast::AwaitExpression, expression, duration);
BOOST_FUSION_ADAPT_STRUCT(ez::flow::ast::TryExpression, expression);

BOOST_FUSION_ADAPT_STRUCT(ez::flow::ast::VariableDeclaration, name, type, expression);
BOOST_FUSION_ADAPT_STRUCT(ez::flow::ast::AssignmentStatement, path, expression);

BOOST_FUSION_ADAPT_STRUCT(ez::flow::ast::WorkflowDefinition, name, signature, statements);

BOOST_FUSION_ADAPT_STRUCT(ez::flow::ast::IfBlock, if_block, elif_blocks, else_block);
BOOST_FUSION_ADAPT_STRUCT(ez::flow::ast::RepeatBlock, scope);
BOOST_FUSION_ADAPT_STRUCT(ez::flow::ast::ConditionBlock, condition, statements);
