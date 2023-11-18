#pragma once

#include "evaluation_context.hpp"

#include <ez/flow/statement.hpp>

#include <ez/async/task.hpp>

namespace ez::flow::engine {

using async::Task;

Entity eval(EvaluationContext&, Statement<ast::IdentifierPath>);
Entity eval(EvaluationContext&, Statement<ast::Literal>);
Entity eval(EvaluationContext&, Statement<ast::FunctionCall>);
Entity eval(EvaluationContext&, Statement<ast::Array>);
Entity eval(EvaluationContext&, Statement<ast::Dictionary>);

Entity eval(EvaluationContext&, Statement<ast::UnaryExpression>);
Entity eval(EvaluationContext&, Statement<ast::MultiplicativeExpression>);
Entity eval(EvaluationContext&, Statement<ast::AdditiveExpression>);
Entity eval(EvaluationContext&, Statement<ast::RelationalExpression>);
Entity eval(EvaluationContext&, Statement<ast::EqualityExpression>);
Entity eval(EvaluationContext&, Statement<ast::Expression>);

Entity eval(EvaluationContext&, Statement<ast::AwaitExpression>);
Entity eval(EvaluationContext&, Statement<ast::TryExpression>);
Entity eval(EvaluationContext&, Statement<ast::PrimaryExpression>);

Entity eval(EvaluationContext&, WorkflowInvocation);

void eval(EvaluationContext&, Statement<ast::WorkflowStatement>);
void eval(EvaluationContext&, Statement<ast::ImportStatement>);
void eval(EvaluationContext&, Statement<ast::VariableDeclaration>);
void eval(EvaluationContext&, Statement<ast::WorkflowDefinition>);
void eval(EvaluationContext&, Statement<ast::AssignmentStatement>);
void eval(EvaluationContext&, Statement<ast::DelayStatement>);
void eval(EvaluationContext&, Statement<ast::ReturnStatement>);
void eval(EvaluationContext&, Statement<ast::RaiseStatement>);
void eval(EvaluationContext&, Statement<ast::IfBlock>);
void eval(EvaluationContext&, Statement<ast::WorkflowScope>);

void eval(EvaluationContext&, Statement<ast::Program>);
void eval(EvaluationContext&, const Program<ast::Program>&);

}  // namespace ez::flow::engine
