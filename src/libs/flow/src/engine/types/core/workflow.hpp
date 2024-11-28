#pragma once

#include "../core/value_interface.hpp"

#include <ez/flow/statement.hpp>

#include <ez/Cow.hpp>

namespace ez::flow::ast {
struct WorkflowDefinition;

}  // namespace ez::flow::ast

namespace ez::flow::engine {
struct WorkflowDefinition : Value<> {
    EZ_FLOW_TYPE(EntityCategory::WorkflowDefinition)
    Statement<ast::WorkflowDefinition> statement;
};

struct WorkflowInvocation : Value<> {
    EZ_FLOW_TYPE(EntityCategory::WorkflowInvocation)
    WorkflowDefinition definition;
    OneOf<Statement<ast::FunctionCall>, Statement<ast::Expression>> instanciation;
    Cow<CallArguments> arguments;
};

}  // namespace ez::flow::engine
