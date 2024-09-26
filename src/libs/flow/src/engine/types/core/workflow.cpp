#include "workflow.hpp"

#include "../entity_utils.hpp"

namespace ez::flow::engine {
EZ_FLOW_TYPE_IMPL(WorkflowDefinition)
{
    Type result;
    result.name = "workflow";
    result.representation.call = [](const Entity& workflow) -> std::string {
        Statement<ast::WorkflowDefinition> statement = workflow.as<WorkflowDefinition>().statement;
        return std::format("Workflow '{}' defined in {}", std::string_view{statement.ast().name},
                           statement.program().file_path);
    };
    return result;
}

EZ_FLOW_TYPE_IMPL(WorkflowInvocation)
{
    Type result;
    result.name = "workflow invocation";
    result.representation.call = [](const Entity&) -> std::string { return "Workflow invokation"; };
    return result;
}

}  // namespace ez::flow::engine
