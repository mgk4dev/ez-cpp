#include "interpreter.hpp"

#include "../types/entity_utils.hpp"

namespace ez::flow::engine {

Task<> Interpreter::eval(Statement<ast::Program> program)
{
    for (const auto& statement : program.ast()) {
        auto res = eval(program >> statement);
        co_await res;
        if (evaluation_scope.current().flow_control_policy != FlowControlPolicy::Stay) break;
    }
}

Task<> Interpreter::eval(const Program<ast::Program>& program)
{
    auto statement = make_statement(program, program.ast);
    co_await eval(statement);
}

}  // namespace ez::flow::engine
