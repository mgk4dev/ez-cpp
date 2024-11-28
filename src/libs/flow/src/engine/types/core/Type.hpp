#pragma once

#include "Function.hpp"

#include <functional>
#include <map>

namespace ez::flow::engine {
struct Type {
    std::string id = "undefined";
    std::string name = "undefined";

    std::map<std::string, MemberFunction, std::less<>> member_functions;
    std::map<std::string, StaticProperty, std::less<>> static_properties;
    DynamicProperties dynamic_properties;

    Representation representation;
    Constructor construct;
    Assignment assign;
    Convert convert;

    UnaryOp unary_op_plus{"+"};
    UnaryOp unary_op_minus{"-"};
    UnaryOp unary_op_not{"!"};

    BinaryOp binary_op_mult{"*"};
    BinaryOp binary_op_div{"/"};
    BinaryOp binary_op_plus{"+"};
    BinaryOp binary_op_minus{"-"};

    BinaryOp binary_op_less{"<"};
    BinaryOp binary_op_less_eq{"<="};
    BinaryOp binary_op_greater{">"};
    BinaryOp binary_op_greater_eq{">="};

    BinaryOp binary_op_eq{"=="};
    BinaryOp binary_op_not_eq{"!="};
    BinaryOp binary_op_contains{"in"};

    BinaryOp binary_op_and{"and"};
    BinaryOp binary_op_or{"or"};

    EvalResult get_property(Entity&, std::string_view name) const;
    EvalResult call_mem_function(Entity&, std::string_view name, CallArguments) const;
};

// types are static variables
inline bool same_type(const Type& lhs, const Type& rhs) { return lhs.id == rhs.id; }

}  // namespace ez::flow::engine
