#include "eval.hpp"

#include "../conversion/json.hpp"
#include "../types/entity_utils.hpp"

#include <ez/scope_guard.hpp>

namespace ez::flow::engine {

void eval(EvaluationContext& context, Statement<ast::ImportStatement>)
{
    context.logger.trace("Imports are not supported yet");
}

void eval(EvaluationContext& context, Statement<ast::VariableDeclaration> statement)
{
    auto exists = context.scope.current().find_entity(statement.ast().name);
    if (exists) {
        throw EvaluationError{
            std::format("'{}' already declaraed in the current scope ", statement.ast().name),
            statement.ast()};
    }

    auto* type = context.scope.find_type(statement.ast().type);

    if (!type) {
        throw EvaluationError{
            std::format("Unknown type '{}'", std::string_view{statement.ast().type}),
            statement.ast().type};
    }
    Entity arguments = Void{};
    if (statement.ast().expression)
        arguments = eval(context, statement >> statement.ast().expression.value());
    EvalResult value = entity::construct(*type, std::move(arguments).copy());
    context.scope.current().add(statement.ast().name,
                                entity::ensure_value(std::move(value), statement.ast()));
}

void eval(EvaluationContext& context, Statement<ast::WorkflowDefinition> statement)
{
    auto exists = context.scope.current().find_entity(statement.ast().name);

    if (exists) {
        throw EvaluationError{
            std::format("'{}' already declaraed in the current scope ", statement.ast().name),
            statement.ast()};
    }

    WorkflowDefinition workflow;
    workflow.statement = statement;
    context.scope.current().add(statement.ast().name, std::move(workflow));
}

void eval(EvaluationContext& context, Statement<ast::AssignmentStatement> statement)
{
    auto lhs = eval(context, statement >> statement.ast().path);
    auto expression = eval(context, statement >> statement.ast().expression);
    entity::assign(lhs, expression);
}

void eval(EvaluationContext& context, Statement<ast::DelayStatement> statement)
{
    auto value = eval(context, statement >> statement.ast().expression);
}

void eval(EvaluationContext& context, Statement<ast::ReturnStatement> statement)
{
    context.scope.current().return_value = eval(context, statement >> statement.ast().expression);
    context.scope.current().flow_control_policy = FlowControlPolicy::ExitCallScope;
}

void eval(EvaluationContext& context, Statement<ast::RaiseStatement> statement)
{
    Error error;
    auto expression = eval(context, statement >> statement.ast().expression);

    if (expression.is<String>()) { error.what = expression.as<String>().value(); }
    else if (expression.is<Error>()) {
        error = expression.as<Error>();
    }
    else {
        throw EvaluationError::build(statement.ast().expression,
                                     "Expecting a string or error expression");
    }

    context.scope.current().return_value = std::move(error);

    context.scope.current().flow_control_policy = FlowControlPolicy::ExitCallScope;
}

void eval(EvaluationContext& context, Statement<ast::WorkflowStatement> statement)
{
    return statement.ast().match(
        [&]<typename T>(const ast::Forward<T>& workflow_statement) {
            eval(context, statement >> workflow_statement.get());
        },
        [&](const auto& workflow_statement) {
            eval(context, statement >> workflow_statement);
        });
}

void eval(EvaluationContext& context, Statement<ast::WorkflowScope> scope)
{
    context.scope.push(PushScopeMode::Inherit);
    EZ_ON_SCOPE_EXIT { context.scope.pop(); };

    for (const ast::WorkflowStatement& statement : scope.ast()) {
        eval(context, scope >> statement);
        if (context.scope.current().flow_control_policy != FlowControlPolicy::NoExit) return;
    }
}

void eval(EvaluationContext& context, Statement<ast::IfBlock> statement)
{
    auto eval_condition = [](EvaluationContext& context,
                             Statement<ast::Expression> condition_statement,
                             Statement<ast::WorkflowScope> scope, bool& evaluated) {
        Entity condition = eval(context, condition_statement);

        if (!condition.is<Boolean>())
            condition = entity::ensure_value(entity::convert(condition, Boolean::static_type()),
                                             condition_statement.ast());

        if (!condition.as<Boolean>().value()) { evaluated = false; }
        evaluated = true;
        return eval(context, scope);
    };

    bool evaluated;

    eval_condition(context, statement >> statement.ast().if_block.condition,
                   statement >> statement.ast().if_block.statements, evaluated);

    if (context.scope.current().flow_control_policy != FlowControlPolicy::NoExit) return;

    if (!evaluated) {
        for (const auto& block : statement.ast().elif_blocks) {
            eval_condition(context, statement >> block.condition, statement >> block.statements,
                           evaluated);
            if (evaluated) return;
        }
    }

    if (evaluated || statement.ast().else_block.empty()) return;

    return eval(context, statement >> statement.ast().else_block);
}

Entity eval(EvaluationContext& context, Statement<ast::AwaitExpression> statement)
{
    Entity awaitable = eval(context, statement >> statement.ast().expression);

    if (awaitable.is<WorkflowInvocation>()) {
        return eval(context, awaitable.as<WorkflowInvocation>());
    }

    throw EvaluationError{
        std::format("Expression of type '{}' in not an awaitable", awaitable.type().name),
        statement.ast()};
}

Entity eval(EvaluationContext& context, Statement<ast::TryExpression> statement)
{
    Entity expression = eval(context, statement >> statement.ast().expression);

    if (expression.is<Error>()) {
        context.scope.current().return_value = std::move(expression);
        context.scope.current().flow_control_policy = FlowControlPolicy::ExitCallScope;
    }

    return expression;
}

Entity eval(EvaluationContext& context, WorkflowInvocation awaitable)
{
    const Type* return_type = &Void::static_type();

    if (awaitable.definition.statement.ast().signature &&
        awaitable.definition.statement.ast().signature.value().return_type) {
        std::string_view return_type_name =
            awaitable.definition.statement.ast().signature.value().return_type.value();

        return_type = context.scope.find_type(return_type_name);

        if (!return_type) {
            throw EvaluationError::build(
                awaitable.definition.statement.ast().signature.value().return_type.value(),
                "Unknown return type {} not found", return_type_name);
        }
    }

    auto& scope = context.scope.push(PushScopeMode::New);

    const CallArguments& arguments = awaitable.arguments.value();

    if (auto&& signature = awaitable.definition.statement.ast().signature; signature) {
        const ast::DeclarationList& argument_declaration = signature->inputs;

        if (arguments.size() != argument_declaration.size()) {
            throw EvaluationError{
                std::format("Invalid argument count: expected {} arguments", arguments.size()),
                awaitable.instanciation.ast().arguments};
        }

        uint index = 0;
        for (const ast::Declaration& input : argument_declaration) {
            const Entity* argument = arguments.find(input.name);
            if (!argument) argument = &arguments.at(index).value;

            auto expected_type = context.scope.find_type(input.type);

            if (!expected_type) {
                throw EvaluationError::build(awaitable.instanciation.ast().arguments,
                                             "Type {} not found", argument_declaration.at(0).type);
            }

            if (!same_type(*expected_type, argument->type())) {
                throw EvaluationError::build(
                    awaitable.instanciation.ast().arguments.at(index),
                    "Type mismatch for argument '{}': expected a '{}' while got a '{}'",
                    std::string_view{input.name}, std::string_view{input.type},
                    argument->type().name);
            }

            context.scope.current().add(input.name, *argument);
            ++index;
        }
    }

    eval(context,
         awaitable.definition.statement >> awaitable.definition.statement.ast().statements);

    if (!scope.return_value.value() && !same_type(*return_type, Void::static_type())) {
        throw EvaluationError::build(awaitable.instanciation.ast().path,
                                     "Workflow did not return any value");
    }

    Entity result = scope.return_value.value() | Void{};

    if (!same_type(result.type(), *return_type) &&
        !same_type(result.type(), Error::static_type())) {
        throw EvaluationError::build(awaitable.instanciation.ast().path,
                                     "Invalid return type: expected a {}, got a {}",
                                     return_type->name, result.type().name);
    }

    context.scope.pop();

    return result;
}

}  // namespace ez::flow::engine
