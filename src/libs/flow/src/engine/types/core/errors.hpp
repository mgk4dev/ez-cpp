#pragma once

#include <ez/flow/ast.hpp>
#include <ez/Result.hpp>

#include <format>
#include <stdexcept>
#include <string_view>

namespace ez::flow::engine {

class EvaluationError : public std::runtime_error {
public:
    ast::Located location;

    using std::runtime_error::runtime_error;

    EvaluationError(std::string_view msg, ast::Located where)
        : std::runtime_error{msg.data()}, location{where}
    {
    }

    EvaluationError& operator<<(ast::Located location)
    {
        this->location = location;
        return *this;
    }

    EvaluationError operator<<(ast::Located location) const
    {
        auto self = *this;
        self.location = location;
        return self;
    }

    template <typename... Args>
    static EvaluationError build(const ast::Located& location,
                                 std::format_string<Args...> base,
                                 Args&&... args)
    {
        return EvaluationError{std::format(base, EZ_FWD(args)...)} << location;
    }
};

namespace error {

template <typename... Args>
inline auto generic(std::format_string<Args...> base, Args&&... args)
{
    return Fail{EvaluationError{std::format(base, EZ_FWD(args)...)}};
}

inline auto op_not_found(std::string_view op, std::string_view lhs_type, std::string_view rhs_type)
{
    return Fail{EvaluationError{std::format(
        "No matching for operator '{}' for types '{}' and '{}' ", op, lhs_type, rhs_type)}};
}

inline auto op_not_found(std::string_view op, std::string_view type)
{
    return Fail{
        EvaluationError{std::format("No matching for operator '{}' for type '{}'", op, type)}};
}

inline auto type_not_constructible() { return Fail{EvaluationError{"Type not constructible"}}; }
inline auto type_not_convertible(std::string_view source_type, std::string_view destination_type)
{
    return Fail{EvaluationError{
        std::format("Type '{}' not convertible to '{}'", source_type, destination_type)}};
}

inline auto call_op_not_found() { return Fail{EvaluationError{"Call operator not available"}}; }

inline auto undefined_type() { return Fail{EvaluationError{"Undefined type"}}; }

inline auto property_not_found(std::string_view property_name)
{
    return Fail{EvaluationError{std::format("Property '{}' not found", property_name)}};
}

inline auto member_function_not_found(std::string_view name)
{
    return Fail{EvaluationError{std::format("Member function '{}' not found", name)}};
}

inline auto invalid_assignment(std::string_view expected_type, std::string_view actual_type)
{
    return Fail{EvaluationError{std::format(
        "Invalid assignent. Expected '{}' while provided a '{}'", expected_type, actual_type)}};
}

inline auto invalid_constructor(std::string_view expected_type, std::string_view actual_type)
{
    return Fail{EvaluationError{std::format(
        "Invalid construction. Expected '{}' while provided a '{}'", expected_type, actual_type)}};
}

inline auto invalid_function_call(std::string_view type,
                                  std::string_view fn_name,
                                  std::string_view reason)
{
    return Fail{EvaluationError{std::format("Invalid call of '{}.{}': {}", type, fn_name, reason)}};
}

}  // namespace error
}  // namespace ez::flow::engine
