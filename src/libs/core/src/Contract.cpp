#include <ez/Contract.hpp>

#include <format>
#include <iostream>
#include <string>

namespace ez::contract {
std::string to_string(const Expression& expression)
{
    return std::format("Condition '{}' failed in {}, line {}", expression.value, expression.file,
                       expression.line);
}

void print_failed_expression(const Expression& expression)
{
    std::cerr << to_string(expression) << std::endl;
}

void Terminate::operator()(const Expression& expression) const
{
    print_failed_expression(expression);
    std::terminate();
}

void ThrowLogicError::operator()(const Expression& expression) const
{
    print_failed_expression(expression);
    throw std::logic_error(to_string(expression));
}

void PrintError::operator()(const Expression& expression) const
{
    print_failed_expression(expression);
}


}  // namespace ez::contract
