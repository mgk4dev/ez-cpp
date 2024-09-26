#pragma once

#include "evaluation_context.hpp"

#include <ez/flow/extensions.hpp>
#include <ez/flow/statement.hpp>

#include <ez/async/Executor.hpp>
#include <ez/async/Task.hpp>

namespace ez::flow::engine {
using async::Task;

struct Interpreter {
    Ref<async::IoContext> io_context;
    struct {
        Logger logger;
        ext::ActionRequest run_action_delegate;
        ext::CampaignRequest run_campaign_delegate;
        ext::DeviceInfoRequest request_device_info_delegate;
        ext::HttpRequest http_request_delegate;
    } extensions;

    EvaluationScope evaluation_scope;
    struct {
        std::uint32_t max_loop_count = 1000;
    } options;

    Interpreter(async::IoContext& ctx) : io_context{ctx} {}

    void set_instance_id(uint);

    Task<CallArguments> eval_call_arguments(Statement<ast::FunctionCall> statement);

    Task<Entity> eval(Statement<ast::IdentifierPath>);
    Task<Entity> eval(Statement<ast::Literal>);
    Task<Entity> eval(Statement<ast::FunctionCall>);
    Task<Entity> eval(Statement<ast::Array>);
    Task<Entity> eval(Statement<ast::Dictionary>);

    Task<Entity> eval(Statement<ast::UnaryExpression>);
    Task<Entity> eval(Statement<ast::MultiplicativeExpression>);
    Task<Entity> eval(Statement<ast::AdditiveExpression>);
    Task<Entity> eval(Statement<ast::RelationalExpression>);
    Task<Entity> eval(Statement<ast::EqualityExpression>);
    Task<Entity> eval(Statement<ast::Expression>);

    Task<Entity> eval(Statement<ast::AwaitExpression>);
    Task<Entity> eval_timeout(Statement<ast::Expression>);
    Task<Entity> eval_await(Statement<ast::AwaitExpression> statement, const WorkflowInvocation&);
    Task<Entity> eval_await(Statement<ast::AwaitExpression> statement, const Action&);
    Task<Entity> eval_await(Statement<ast::AwaitExpression> statement, const Campaign&);
    Task<Entity> eval_await(Statement<ast::AwaitExpression> statement, const DeviceInfoRequest&);
    Task<Entity> eval_await(Statement<ast::AwaitExpression> statement, const HttpRequest&);

    Task<Entity> eval(Statement<ast::TryExpression>);
    Task<Entity> eval(Statement<ast::PrimaryExpression>);

    Task<Entity> eval(WorkflowInvocation);

    Task<> eval_condition(Statement<ast::Expression> condition_statement,
                          Statement<ast::WorkflowScope> scope,
                          bool& evaluated);

    Task<> eval(Statement<ast::WorkflowStatement>);
    Task<> eval(Statement<ast::ImportStatement>);
    Task<> eval(Statement<ast::VariableDeclaration>);
    Task<> eval(Statement<ast::WorkflowDefinition>);
    Task<> eval(Statement<ast::AssignmentStatement>);
    Task<> eval(Statement<ast::DelayStatement>);
    Task<> eval(Statement<ast::ReturnStatement>);
    Task<> eval(Statement<ast::RaiseStatement>);
    Task<> eval(Statement<ast::BreakStatement>);
    Task<> eval(Statement<ast::IfBlock>);
    Task<> eval(Statement<ast::RepeatBlock>);
    Task<> eval(Statement<ast::WorkflowScope>, ScopeType);

    Task<> eval(Statement<ast::Program>);
    Task<> eval(const Program<ast::Program>&);
};

}  // namespace ez::flow::engine
