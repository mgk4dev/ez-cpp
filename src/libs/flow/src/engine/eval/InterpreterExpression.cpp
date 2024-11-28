
#include "../types/EntityUtils.hpp"
#include "../types/core/CallArguments.hpp"

#include "Literals.hpp"

namespace ez::flow::engine {
Task<Entity> Interpreter::eval(Statement<ast::Literal> statement)
{
    co_return statement.ast().apply_visitor([&](const auto& literal) { return to_value(literal); });
}

Task<Entity> Interpreter::eval(Statement<ast::IdentifierPath> statement)
{
    const ast::IdentifierPath& path = statement.ast();

    auto entry = evaluation_scope.find_entity(path.front());

    if (!entry) {
        throw EvaluationError(
            std::format("Use of undeclared identifier '{}'", std::string_view{path.front()}), path);
    }

    Entity result = entry->value;

    for (const auto& identifier : std::ranges::subrange{path.begin() + 1, path.end()}) {
        result = entity::ensure_value(entity::get_property(result, identifier), identifier);
    }

    co_return result;
}

void Interpreter::set_instance_id(unsigned int id)
{
    evaluation_scope.current().add("instance_id", Integer{id});
}

Task<CallArguments> Interpreter::eval_call_arguments(Statement<ast::FunctionCall> statement)
{
    CallArguments args;
    args.reserve(statement.ast().arguments.size());

    for (const auto& ast_argument : statement.ast().arguments) {
        if (ast_argument.name)
            args.push(ast_argument.name.value(), co_await eval(statement >> ast_argument.value));
        else
            args.push(co_await eval(statement >> ast_argument.value));
    }

    co_return args;
}

Task<Entity> Interpreter::eval(Statement<ast::FunctionCall> statement)
{
    auto callable = co_await eval(statement >> statement.ast().path);

    if (callable.category() == EntityCategory::FreeFunction) {
        co_return entity::ensure_value(
            callable.as<FreeFunction>().call(co_await eval_call_arguments(statement)),
            statement.ast());
    }

    if (callable.category() == EntityCategory::WorkflowDefinition) {
        WorkflowInvocation task;
        task.definition = callable.as<WorkflowDefinition>();
        task.instanciation = statement;
        task.arguments = co_await eval_call_arguments(statement);
        co_return task;
    }

    if (callable.category() == EntityCategory::MemberFunction) {
        auto path = statement.ast().path;
        path.pop_back();
        auto self = co_await eval(statement >> path);
        auto result = entity::call_mem_function(self, statement.ast().path.back(),
                                                co_await eval_call_arguments(statement));
        co_return entity::ensure_value(std::move(result), statement.ast());
    }

    throw EvaluationError(
        std::format("'{}' is not a callable", (statement >> statement.ast().path).get_code()),
        statement.ast().path);

    co_return Entity{};
}

Task<Entity> Interpreter::eval(Statement<ast::Array> statement)
{
    Array array;
    array->reserve(statement.ast().size());
    for (auto&& elem : statement.ast()) { array->push_back(co_await eval(statement >> elem)); }

    co_return array;
}

Task<Entity> Interpreter::eval(Statement<ast::Dictionary> statement)
{
    Dictionary dict;
    for (auto&& elem : statement.ast()) {
        dict->insert({elem.name, co_await eval(statement >> elem.value)});
    }
    co_return dict;
}

Task<Entity> Interpreter::eval(Statement<ast::PrimaryExpression> statement)
{
    auto visitor = [&](const auto& stmt) -> Task<Entity> {
        return eval(statement >> ast::unwrap(stmt));
    };
    co_return co_await statement.ast().apply_visitor(visitor);
}

Task<Entity> Interpreter::eval(Statement<ast::UnaryExpression> statement)
{
    auto value = co_await eval(statement >> statement.ast().expression);

    if (!statement.ast().op) co_return value;

    const auto& result_type = value.type();
    auto result = [&] {
        switch (statement.ast().op.value()) {
            case ast::UnaryOperator::Plus: return result_type.unary_op_plus(value);
            case ast::UnaryOperator::Minus: return result_type.unary_op_minus(value);
            case ast::UnaryOperator::Not: return result_type.unary_op_not(value);
        }
        std::terminate();
    }();

    co_return entity::ensure_value(std::move(result), statement.ast());
}
Task<Entity> Interpreter::eval(Statement<ast::MultiplicativeExpression> statement)
{
    Entity result = co_await eval(statement >> statement.ast().first);

    for (const ast::Operation<ast::UnaryExpression, ast::MultiplicativeOperator>& ast_term :
         statement.ast().terms) {
        auto term_value = co_await eval(statement >> ast_term.term);
        const auto result_type = result.type();

        auto operation_result = [&] {
            switch (ast_term.op) {
                case ast::MultiplicativeOperator::Mult:
                    return result_type.binary_op_mult(result, term_value);
                case ast::MultiplicativeOperator::Div:
                    return result_type.binary_op_div(result, term_value);
            }
            std::terminate();
        }();

        result = entity::ensure_value(std::move(operation_result), ast_term.term);
    }

    co_return result;
}

Task<Entity> Interpreter::eval(Statement<ast::AdditiveExpression> statement)
{
    Entity lhs = co_await eval(statement >> statement.ast().first);

    for (const ast::Operation<ast::MultiplicativeExpression, ast::AdditiveOperator>& ast_term :
         statement.ast().terms) {
        auto rhs = co_await eval(statement >> ast_term.term);
        const auto result_type = lhs.type();

        auto operation_result = [&] {
            switch (ast_term.op) {
                case ast::AdditiveOperator::Plus: return result_type.binary_op_plus(lhs, rhs);
                case ast::AdditiveOperator::Minus: return result_type.binary_op_minus(lhs, rhs);
            }
            std::terminate();
        }();

        lhs = entity::ensure_value(std::move(operation_result), ast_term.term);
    }

    co_return lhs;
}

Task<Entity> Interpreter::eval(Statement<ast::RelationalExpression> statement)
{
    Entity lhs = co_await eval(statement >> statement.ast().first);

    for (const ast::Operation<ast::AdditiveExpression, ast::RelationalOperator>& ast_term :
         statement.ast().terms) {
        auto rhs = co_await eval(statement >> ast_term.term);
        const auto result_type = lhs.type();

        auto operation_result = [&] {
            switch (ast_term.op) {
                case ast::RelationalOperator::Less: return result_type.binary_op_less(lhs, rhs);
                case ast::RelationalOperator::LessEq:
                    return result_type.binary_op_less_eq(lhs, rhs);
                case ast::RelationalOperator::Greater:
                    return result_type.binary_op_greater(lhs, rhs);
                case ast::RelationalOperator::GreaterEq:
                    return result_type.binary_op_greater_eq(lhs, rhs);
            }
            std::terminate();
        }();

        lhs = entity::ensure_value(std::move(operation_result), ast_term.term);
    }

    co_return lhs;
}

std::string to_type_name(const ast::RelationalExpression& expression)
{
    const ast::PrimaryExpression& primary_expression = expression.first.first.first.expression;

    if (primary_expression.is<ast::IdentifierPath>()) {
        return primary_expression.as<ast::IdentifierPath>().front();
    }

    return "<unknow type>";
}

Task<Entity> Interpreter::eval(Statement<ast::EqualityExpression> statement)
{
    Entity lhs = co_await eval(statement >> statement.ast().first);

    for (const ast::Operation<ast::RelationalExpression, ast::EqualityOperator>& ast_term :
         statement.ast().terms) {
        if (ast_term.op == ast::EqualityOperator::Is) {
            std::string type_name = to_type_name(ast_term.term);
            auto* type = evaluation_scope.find_type(type_name);
            if (!type) {
                throw EvaluationError::build(ast_term.term, "'{}' is not a type", type_name);
            }
            lhs = Boolean{same_type(lhs.type(), *type)};
        }
        else {
            auto rhs = co_await eval(statement >> ast_term.term);
            const auto result_type = lhs.type();

            auto operation_result = [&] {
                switch (ast_term.op) {
                    case ast::EqualityOperator::Eq: return result_type.binary_op_eq(lhs, rhs);
                    case ast::EqualityOperator::NotEq:
                        return result_type.binary_op_not_eq(lhs, rhs);
                    case ast::EqualityOperator::In: return rhs.type().binary_op_contains(rhs, lhs);
                    case ast::EqualityOperator::Is: assert(false);
                }
                std::terminate();
            }();
            lhs = entity::ensure_value(std::move(operation_result), ast_term.term);
        }
    }

    co_return lhs;
}

Task<Entity> Interpreter::eval(Statement<ast::Expression> statement)
{
    Entity lhs = co_await eval(statement >> statement.ast().first);

    for (const ast::Operation<ast::EqualityExpression, ast::LogicalOperator>& ast_term :
         statement.ast().terms) {
        auto rhs = co_await eval(statement >> ast_term.term);
        const auto& result_type = lhs.type();

        auto operation_result = [&] {
            switch (ast_term.op) {
                case ast::LogicalOperator::And: return result_type.binary_op_and(lhs, rhs);
                case ast::LogicalOperator::Or: return result_type.binary_op_or(lhs, rhs);
            }
            std::terminate();
        }();

        lhs = entity::ensure_value(std::move(operation_result), ast_term.term);
    }

    co_return lhs;
}

}  // namespace ez::flow::engine
