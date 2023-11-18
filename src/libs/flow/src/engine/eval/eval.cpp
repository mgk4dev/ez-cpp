#include "eval.hpp"

#include "../types/entity_utils.hpp"

namespace ez::flow::engine {

void eval(EvaluationContext& context, Statement<ast::Program> program)
{
    for (const auto& statement : program.ast()) {
        eval(context, program >> statement);
        if (context.scope.current().flow_control_policy != FlowControlPolicy::NoExit) break;
    }
}

void eval(EvaluationContext& context, const Program<ast::Program>& program)
{
    auto statement = make_statement(program, program.ast);
    eval(context, statement);
}

}  // namespace ez::flow::engine
