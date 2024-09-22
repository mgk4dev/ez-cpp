#include "interpreter.hpp"

#include "../conversion/json.hpp"
#include "../types/entity_utils.hpp"

#include <ez/async/Delay.hpp>
#include <ez/async/WhenAny.hpp>

#include <ez/ScopeGuard.hpp>
#include <ez/Traits.hpp>

#include <iostream>

namespace ez::flow::engine {

Task<> Interpreter::eval(Statement<ast::ImportStatement>)
{
    extensions.logger.trace("Imports are not supported yet");
    co_return;
}

Task<> Interpreter::eval(Statement<ast::VariableDeclaration> statement)
{
    auto exists = evaluation_scope.current().find_entity(statement.ast().name);
    if (exists) {
        throw EvaluationError{
            std::format("'{}' already declaraed in the current scope ", statement.ast().name.raw()),
            statement.ast()};
    }

    auto* type = evaluation_scope.find_type(statement.ast().type);

    if (!type) {
        throw EvaluationError{
            std::format("Unknown type '{}'", std::string_view{statement.ast().type}),
            statement.ast().type};
    }
    Entity arguments = Void{};
    if (statement.ast().expression)
        arguments = co_await eval(statement >> statement.ast().expression.value());
    EvalResult value = entity::construct(*type, std::move(arguments).copy());
    evaluation_scope.current().add(statement.ast().name,
                                   entity::ensure_value(std::move(value), statement.ast()));
}

Task<> Interpreter::eval(Statement<ast::WorkflowDefinition> statement)
{
    auto exists = evaluation_scope.current().find_entity(statement.ast().name);

    if (exists) {
        throw EvaluationError{
            std::format("'{}' already declaraed in the current scope ", statement.ast().name.raw()),
            statement.ast()};
    }

    WorkflowDefinition workflow;
    workflow.statement = statement;
    evaluation_scope.current().add(statement.ast().name, std::move(workflow));

    co_return;
}

Task<> Interpreter::eval(Statement<ast::AssignmentStatement> statement)
{
    auto lhs = co_await eval(statement >> statement.ast().path);
    auto expression = co_await eval(statement >> statement.ast().expression);
    auto r = entity::assign(lhs, expression);
    unused(r);
}

Task<> Interpreter::eval(Statement<ast::DelayStatement> statement)
{
    auto timeout = co_await eval(statement >> statement.ast().expression);

    if (!timeout.is<Duration>()) {
        throw EvaluationError::build(statement.ast().expression, "Expected a '{}' got a '{}' ",
                                     Duration::static_type().name, timeout.type().name);
    }

    co_await async::delay(io_context.get(), timeout.as<Duration>().value().to_std_duration());
}

Task<> Interpreter::eval(Statement<ast::ReturnStatement> statement)
{
    evaluation_scope.return_value(co_await eval(statement >> statement.ast().expression));
}

Task<> Interpreter::eval(Statement<ast::BreakStatement>)
{
    evaluation_scope.break_loop();
    co_return;
}

Task<> Interpreter::eval(Statement<ast::RaiseStatement> statement)
{
    Error error;
    auto expression = co_await eval(statement >> statement.ast().expression);

    if (expression.is<String>()) { error.what = expression.as<String>().value(); }
    else if (expression.is<Error>()) {
        error = expression.as<Error>();
    }
    else {
        throw EvaluationError::build(statement.ast().expression,
                                     "Expecting a string or error expression");
    }

    evaluation_scope.return_value(std::move(error));
}

Task<> Interpreter::eval(Statement<ast::WorkflowStatement> statement)
{
    auto visitor = [&](const auto& workflow_statement) -> Task<> {
        co_await eval(statement >> ast::unwrap(workflow_statement));
    };
    co_await statement.ast().apply_visitor(visitor);
}

Task<> Interpreter::eval(Statement<ast::WorkflowScope> scope_statement, ScopeType scope_type)
{
    evaluation_scope.push(scope_type);
    EZ_ON_SCOPE_EXIT { evaluation_scope.pop(); };

    for (const ast::WorkflowStatement& statement : scope_statement.ast()) {
        co_await eval(scope_statement >> statement);
        if (evaluation_scope.current().flow_control_policy != FlowControlPolicy::Stay) co_return;
    }
}

Task<> Interpreter::eval_condition(Statement<ast::Expression> condition_statement,
                                   Statement<ast::WorkflowScope> scope_statement,
                                   bool& evaluated)
{
    Entity condition = co_await eval(condition_statement);

    if (!condition.is<Boolean>())
        condition = entity::ensure_value(entity::convert(condition, Boolean::static_type()),
                                         condition_statement.ast());

    if (!condition.as<Boolean>().value()) {
        evaluated = false;
        co_return;
    }
    evaluated = true;
    co_return co_await eval(scope_statement, ScopeType::Local);
}

Task<> Interpreter::eval(Statement<ast::IfBlock> statement)
{
    bool evaluated;

    co_await eval_condition(statement >> statement.ast().if_block.condition,
                            statement >> statement.ast().if_block.statements, evaluated);

    if (evaluation_scope.current().flow_control_policy != FlowControlPolicy::Stay) co_return;

    if (!evaluated) {
        for (const auto& block : statement.ast().elif_blocks) {
            co_await eval_condition(statement >> block.condition, statement >> block.statements,
                                    evaluated);
            if (evaluated) co_return;
            if (evaluation_scope.current().flow_control_policy != FlowControlPolicy::Stay)
                co_return;
        }
    }

    if (evaluated || statement.ast().else_block.empty()) co_return;

    co_await eval(statement >> statement.ast().else_block, ScopeType::Local);
}

Task<> Interpreter::eval(Statement<ast::RepeatBlock> statement)
{
    auto count = options.max_loop_count;
    while (count--) {
        co_await eval(statement >> statement.ast().scope, ScopeType::Loop);
        if (evaluation_scope.current().flow_control_policy != FlowControlPolicy::Stay) co_return;
    }

   // throw EvaluationError::build(statement.ast(), "Maximum loop count reached");
}

Task<Entity> Interpreter::eval_timeout(Statement<ast::Expression> duration)
{
    auto timeout = co_await eval(duration);

    if (!timeout.is<Duration>()) {
        throw EvaluationError::build(duration.ast(), "Expected a '{}' got a '{}' ",
                                     Duration::static_type().name, timeout.type().name);
    }

    if (timeout.as<Duration>()->count == 0) {
        timeout = Duration{in_place, 1, ast::DurationUnit::Ms};
    }

    co_return timeout;
}

Task<Entity> Interpreter::eval_await(Statement<ast::AwaitExpression> statement,
                                     const WorkflowInvocation& workflow_invocation)
{
    auto timeout = co_await eval_timeout(statement >> statement.ast().duration);
    std::chrono::system_clock::duration timeout_duration =
        timeout.as<Duration>().value().to_std_duration();

    auto result =
        co_await async::when_any(eval(workflow_invocation),
                                 async::delay(io_context.get(), timeout_duration, async::timeout));

    if (result.is<Entity>()) co_return std::move(result.as<Entity>());

    Error error;
    error.what = "Operation timeout";
    error.where.program_ptr = statement.program_ptr;
    error.where.ast_ptr = statement.ast_ptr;
    co_return error;
}

Task<Entity> Interpreter::eval_await(Statement<ast::AwaitExpression> statement,
                                     const Action& action)
{
    auto timeout = co_await eval_timeout(statement >> statement.ast().duration);
    auto request = to_json(action);
    auto task = extensions.run_action_delegate(request);

    auto result = co_await async::when_any(
        task, async::delay(io_context.get(), timeout.as<Duration>().value().to_std_duration(),
                           async::timeout));

    if (result.is<async::TimeoutTag>()) {
        Error error;
        error.what = "Operation timeout";
        error.where.program_ptr = statement.program_ptr;
        error.where.ast_ptr = statement.ast_ptr;
        co_return error;
    }

    ActionReply reply;
    reply = from_json(result.as<JsonObject>());
    co_return Entity{std::move(reply)};
}

Task<Entity> Interpreter::eval_await(Statement<ast::AwaitExpression> statement,
                                     const Campaign& campaign)
{
    auto timeout = co_await eval_timeout(statement >> statement.ast().duration);
    auto request = to_json(campaign);
    auto task = extensions.run_campaign_delegate(request);

    auto result = co_await async::when_any(
        task, async::delay(io_context.get(), timeout.as<Duration>().value().to_std_duration(),
                           async::timeout));

    if (result.is<async::TimeoutTag>()) {
        Error error;
        error.what = "Operation timeout";
        error.where.program_ptr = statement.program_ptr;
        error.where.ast_ptr = statement.ast_ptr;
        co_return error;
    }

    CampaignReply reply;
    reply = from_json(result.as<JsonObject>());
    co_return Entity{std::move(reply)};
}

Task<Entity> Interpreter::eval_await(Statement<ast::AwaitExpression> statement,
                                     const DeviceInfoRequest& device_info)
{
    auto timeout = co_await eval_timeout(statement >> statement.ast().duration);
    auto request = to_json(device_info);
    auto task = extensions.request_device_info_delegate(request);

    auto result = co_await async::when_any(
        task, async::delay(io_context.get(), timeout.as<Duration>().value().to_std_duration(),
                           async::timeout));

    if (result.is<async::TimeoutTag>()) {
        Error error;
        error.what = "Operation timeout";
        error.where.program_ptr = statement.program_ptr;
        error.where.ast_ptr = statement.ast_ptr;
        co_return error;
    }

    DeviceInfoReply reply;
    reply = from_json(result.as<JsonObject>());
    co_return Entity{std::move(reply)};
}

Task<Entity> Interpreter::eval_await(Statement<ast::AwaitExpression> statement,
                                     const HttpRequest& http_request)
{
    auto timeout = co_await eval_timeout(statement >> statement.ast().duration);
    auto request = to_json(http_request);
    auto task = extensions.http_request_delegate(request);

    auto result = co_await async::when_any(
        task, async::delay(io_context.get(), timeout.as<Duration>().value().to_std_duration(),
                           async::timeout));

    if (result.is<async::TimeoutTag>()) {
        Error error;
        error.what = "Operation timeout";
        error.where.program_ptr = statement.program_ptr;
        error.where.ast_ptr = statement.ast_ptr;
        co_return error;
    }

    HttpReply reply;
    reply = from_json(result.as<JsonObject>());
    co_return Entity{std::move(reply)};
}

Task<Entity> Interpreter::eval(Statement<ast::AwaitExpression> statement)
{
    Entity awaitable = co_await eval(statement >> statement.ast().expression);

    if (awaitable.is<WorkflowInvocation>()) {
        co_return co_await eval_await(statement, awaitable.as<WorkflowInvocation>());
    }

    if (awaitable.is<WorkflowDefinition>()) {
        WorkflowInvocation task;
        task.definition = awaitable.as<WorkflowDefinition>();
        task.instanciation = statement >> statement.ast().expression;
        co_return co_await eval_await(statement, task);
    }

    if (awaitable.is<Campaign>()) {
        co_return co_await eval_await(statement, awaitable.as<Campaign>());
    }

    if (awaitable.is<Action>()) {
        co_return co_await eval_await(statement, awaitable.as<Action>());
    }

    if (awaitable.is<DeviceInfoRequest>()) {
        co_return co_await eval_await(statement, awaitable.as<DeviceInfoRequest>());
    }

    if (awaitable.is<HttpRequest>()) {
        co_return co_await eval_await(statement, awaitable.as<HttpRequest>());
    }

    throw EvaluationError{
        std::format("Expression of type '{}' in not an awaitable", awaitable.type().name),
        statement.ast()};
}

Task<Entity> Interpreter::eval(Statement<ast::TryExpression> statement)
{
    Entity expression = co_await eval(statement >> statement.ast().expression);

    if (expression.is<Error>()) { evaluation_scope.return_value(std::move(expression)); }

    co_return expression;
}

Task<Entity> Interpreter::eval(WorkflowInvocation awaitable)
{
    const Type* return_type = &Void::static_type();

    if (awaitable.definition.statement.ast().signature &&
        awaitable.definition.statement.ast().signature.value().return_type) {
        std::string_view return_type_name =
            awaitable.definition.statement.ast().signature.value().return_type.value();

        return_type = evaluation_scope.find_type(return_type_name);

        if (!return_type) {
            throw EvaluationError::build(
                awaitable.definition.statement.ast().signature.value().return_type.value(),
                "Unknown return type {} not found", return_type_name);
        }
    }

    auto& current_scope = evaluation_scope.push(ScopeType::Function);

    const CallArguments& arguments = awaitable.arguments.value();

    if (auto&& signature = awaitable.definition.statement.ast().signature; signature) {
        const ast::DeclarationList& argument_declaration = signature->inputs;

        if (arguments.size() != argument_declaration.size()) {
            throw EvaluationError{
                std::format("Invalid argument count: expected {} arguments", arguments.size()),
                awaitable.instanciation.as<Statement<ast::FunctionCall>>().ast().arguments};
        }

        uint index = 0;
        for (const ast::Declaration& input : argument_declaration) {
            const Entity* argument = arguments.find(input.name);
            if (!argument) argument = &arguments.at(index).value;

            auto expected_type = evaluation_scope.find_type(input.type);

            if (!expected_type) {
                throw EvaluationError::build(
                    awaitable.instanciation.as<Statement<ast::FunctionCall>>().ast().arguments,
                    "Type {} not found", argument_declaration.at(0).type.raw());
            }

            if (!same_type(*expected_type, argument->type())) {
                throw EvaluationError::build(
                    awaitable.instanciation.as<Statement<ast::FunctionCall>>().ast().arguments.at(
                        index),
                    "Type mismatch for argument '{}': expected a '{}' while got a '{}'",
                    std::string_view{input.name}, std::string_view{input.type},
                    argument->type().name);
            }

            evaluation_scope.current().add(input.name, *argument);
            ++index;
        }
    }

    co_await eval(awaitable.definition.statement >> awaitable.definition.statement.ast().statements,
                  ScopeType::Local);

    if (!current_scope.return_value.value() && !same_type(*return_type, Void::static_type())) {
        throw EvaluationError::build(
            awaitable.instanciation.as<Statement<ast::FunctionCall>>().ast().path,
            "Workflow did not return any value");
    }

    Entity result = current_scope.return_value.value() | Void{};

    if (!same_type(result.type(), *return_type) &&
        !same_type(result.type(), Error::static_type())) {
        if (awaitable.instanciation.is<Statement<ast::FunctionCall>>()) {
            throw EvaluationError::build(
                awaitable.instanciation.as<Statement<ast::FunctionCall>>().ast().path,
                "Invalid return type: expected a {}, got a {}", return_type->name,
                result.type().name);
        }
        else if (awaitable.instanciation.is<Statement<ast::Expression>>()) {
            throw EvaluationError::build(
                awaitable.instanciation.as<Statement<ast::Expression>>().ast(),
                "Invalid return type: expected a {}, got a {}", return_type->name,
                result.type().name);
        }
    }

    evaluation_scope.pop();

    co_return result;
}

}  // namespace ez::flow::engine
