
#include "../types/core/call_arguments.hpp"
#include "../types/entity_utils.hpp"

#include "literals.hpp"

#include <iostream>

namespace ez::flow::engine {

Entity eval(EvaluationContext&, Statement<ast::Literal> statement)
{
    return statement.ast().apply_visitor([&](const auto& literal) { return to_value(literal); });
}

Entity eval(EvaluationContext& context, Statement<ast::IdentifierPath> statement)
{
    const ast::IdentifierPath& path = statement.ast();

    auto entry = context.scope.find_entity(path.front());

    if (!entry) {
        throw EvaluationError(
            std::format("Use of undeclared identifier '{}'", std::string_view{path.front()}), path);
    }

    Entity result = entry->value;

    for (const auto& identifier : std::ranges::subrange{path.begin() + 1, path.end()}) {
        result = entity::ensure_value(entity::get_property(result, identifier), identifier);
    }

    return result;
}

CallArguments eval_call_arguments(EvaluationContext& context,
                                  Statement<ast::FunctionCall> statement)
{
    CallArguments args;
    args.reserve(statement.ast().arguments.size());

    for (const auto& ast_argument : statement.ast().arguments) {
        if (ast_argument.name)
            args.push(ast_argument.name.value(), eval(context, statement >> ast_argument.value));
        else
            args.push(eval(context, statement >> ast_argument.value));
    }

    return args;
}

Entity eval(EvaluationContext& context, Statement<ast::FunctionCall> statement)
{
    auto callable = eval(context, statement >> statement.ast().path);

    if (callable.category() == EntityCategory::FreeFunction) {
        return entity::ensure_value(
            callable.as<FreeFunction>().call(eval_call_arguments(context, statement)),
            statement.ast());
    }

    if (callable.category() == EntityCategory::WorkflowDefinition) {
        WorkflowInvocation task;
        task.definition = callable.as<WorkflowDefinition>();
        task.instanciation = statement;
        task.arguments = eval_call_arguments(context, statement);
        return task;
    }

    if (callable.category() == EntityCategory::MemberFunction) {
        auto path = statement.ast().path;
        path.pop_back();
        auto self = eval(context, statement >> path);
        auto result = entity::call_mem_function(self, statement.ast().path.back(),
                                                eval_call_arguments(context, statement));
        return entity::ensure_value(std::move(result), statement.ast());
    }

    throw EvaluationError(
        std::format("'{}' is not a callable", (statement >> statement.ast().path).get_code()),
        statement.ast().path);

    return Entity{};
}

Entity eval(EvaluationContext& context, Statement<ast::Array> statement)
{
    Array array;
    array->reserve(statement.ast().size());
    for (auto&& elem : statement.ast()) { array->push_back(eval(context, statement >> elem)); }

    return array;
}
Entity eval(EvaluationContext& context, Statement<ast::Dictionary> statement)
{
    Dictionary dict;
    for (auto&& elem : statement.ast()) {
        dict->insert({elem.name, eval(context, statement >> elem.value)});
    }
    return dict;
}

Entity eval(EvaluationContext& context, Statement<ast::PrimaryExpression> statement)
{
    return statement.ast().match(
        [&]<typename T>(const ast::Forward<T>& ast_statement) {
            return eval(context, statement >> ast_statement.get());
        },
        [&](const auto& ast_statement) { return eval(context, statement >> ast_statement); });
}

Entity eval(EvaluationContext& context, Statement<ast::UnaryExpression> statement)
{
    auto value = eval(context, statement >> statement.ast().expression);

    if (!statement.ast().op) return value;

    const auto& result_type = value.type();
    auto result = [&] {
        switch (statement.ast().op.value()) {
            case ast::UnaryOperator::Plus: return result_type.unary_op_plus(value);
            case ast::UnaryOperator::Minus: return result_type.unary_op_minus(value);
            case ast::UnaryOperator::Not: return result_type.unary_op_not(value);
        }
    }();

    return entity::ensure_value(std::move(result), statement.ast());
}
Entity eval(EvaluationContext& context, Statement<ast::MultiplicativeExpression> statement)
{
    Entity result = eval(context, statement >> statement.ast().first);

    for (const ast::Operation<ast::UnaryExpression, ast::MultiplicativeOperator>& ast_term :
         statement.ast().terms) {
        auto term_value = eval(context, statement >> ast_term.term);
        const auto result_type = result.type();

        auto operation_result = [&] {
            switch (ast_term.op) {
                case ast::MultiplicativeOperator::Mult:
                    return result_type.binary_op_mult(result, term_value);
                case ast::MultiplicativeOperator::Div:
                    return result_type.binary_op_div(result, term_value);
            }
        }();

        result = entity::ensure_value(std::move(operation_result), ast_term.term);
    }

    return result;
}

Entity eval(EvaluationContext& context, Statement<ast::AdditiveExpression> statement)
{
    Entity lhs = eval(context, statement >> statement.ast().first);

    for (const ast::Operation<ast::MultiplicativeExpression, ast::AdditiveOperator>& ast_term :
         statement.ast().terms) {
        auto rhs = eval(context, statement >> ast_term.term);
        const auto result_type = lhs.type();

        auto operation_result = [&] {
            switch (ast_term.op) {
                case ast::AdditiveOperator::Plus: return result_type.binary_op_plus(lhs, rhs);
                case ast::AdditiveOperator::Minus: return result_type.binary_op_minus(lhs, rhs);
            }
        }();

        lhs = entity::ensure_value(std::move(operation_result), ast_term.term);
    }

    return lhs;
}

Entity eval(EvaluationContext& context, Statement<ast::RelationalExpression> statement)
{
    Entity lhs = eval(context, statement >> statement.ast().first);

    for (const ast::Operation<ast::AdditiveExpression, ast::RelationalOperator>& ast_term :
         statement.ast().terms) {
        auto rhs = eval(context, statement >> ast_term.term);
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
        }();

        lhs = entity::ensure_value(std::move(operation_result), ast_term.term);
    }

    return lhs;
}
Entity eval(EvaluationContext& context, Statement<ast::EqualityExpression> statement)
{
    Entity lhs = eval(context, statement >> statement.ast().first);

    for (const ast::Operation<ast::RelationalExpression, ast::EqualityOperator>& ast_term :
         statement.ast().terms) {
        auto rhs = eval(context, statement >> ast_term.term);
        const auto result_type = lhs.type();

        auto operation_result = [&] {
            switch (ast_term.op) {
                case ast::EqualityOperator::Eq: return result_type.binary_op_eq(lhs, rhs);
                case ast::EqualityOperator::NotEq: return result_type.binary_op_not_eq(lhs, rhs);
                case ast::EqualityOperator::In: return rhs.type().binary_op_contains(rhs, lhs);
            }
        }();

        lhs = entity::ensure_value(std::move(operation_result), ast_term.term);
    }

    return lhs;
}
Entity eval(EvaluationContext& context, Statement<ast::Expression> statement)
{
    Entity lhs = eval(context, statement >> statement.ast().first);

    for (const ast::Operation<ast::EqualityExpression, ast::LogicalOperator>& ast_term :
         statement.ast().terms) {
        auto rhs = eval(context, statement >> ast_term.term);
        const auto& result_type = lhs.type();

        auto operation_result = [&] {
            switch (ast_term.op) {
                case ast::LogicalOperator::And: return result_type.binary_op_and(lhs, rhs);
                case ast::LogicalOperator::Or: return result_type.binary_op_or(lhs, rhs);
            }
        }();

        lhs = entity::ensure_value(std::move(operation_result), ast_term.term);
    }

    return lhs;
}



}  // namespace ez::flow::engine
