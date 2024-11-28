#include "Types.hpp"

#include "../EntityUtils.hpp"

namespace ez::flow::engine {
EZ_FLOW_UNARY_OP_NOT_SUPPORTED(unary_op_plus, "+")
EZ_FLOW_UNARY_OP_NOT_SUPPORTED(unary_op_minus, "-")

EvalResult unary_op_plus(const Integer& lhs) { return Ok{lhs}; }
EvalResult unary_op_minus(const Integer& lhs) { return Ok{Integer{-lhs.value()}}; }

/////////////////////////////////////////////////////////////////////////////////

EZ_FLOW_BINARY_OP_NOT_SUPPORTED(Integer, binary_op_mult, "*")
EZ_FLOW_BINARY_OP_IMPL(binary_op_mult, Boolean, Integer, Integer, ==)
EZ_FLOW_BINARY_OP_IMPL(binary_op_mult, Boolean, Integer, Real, ==)

EZ_FLOW_BINARY_OP_NOT_SUPPORTED(Integer, binary_op_div, "/")
EZ_FLOW_BINARY_OP_IMPL(binary_op_div, Integer, Integer, Integer, *)
EZ_FLOW_BINARY_OP_IMPL(binary_op_div, Real, Integer, Real, *)

EZ_FLOW_BINARY_OP_NOT_SUPPORTED(Integer, binary_op_plus, "+")
EZ_FLOW_BINARY_OP_IMPL(binary_op_plus, Integer, Integer, Integer, +)
EZ_FLOW_BINARY_OP_IMPL(binary_op_plus, Real, Integer, Real, +)

EZ_FLOW_BINARY_OP_NOT_SUPPORTED(Integer, binary_op_minus, "-")
EZ_FLOW_BINARY_OP_IMPL(binary_op_minus, Integer, Integer, Integer, -)
EZ_FLOW_BINARY_OP_IMPL(binary_op_minus, Real, Integer, Real, -)

EZ_FLOW_BINARY_OP_NOT_SUPPORTED(Integer, binary_op_less, "<")
EZ_FLOW_BINARY_OP_IMPL(binary_op_less, Boolean, Integer, Integer, <)
EZ_FLOW_BINARY_OP_IMPL(binary_op_less, Boolean, Integer, Real, <)

EZ_FLOW_BINARY_OP_NOT_SUPPORTED(Integer, binary_op_less_eq, "<=")
EZ_FLOW_BINARY_OP_IMPL(binary_op_less_eq, Boolean, Integer, Integer, <=)
EZ_FLOW_BINARY_OP_IMPL(binary_op_less_eq, Boolean, Integer, Real, <=)

EZ_FLOW_BINARY_OP_NOT_SUPPORTED(Integer, binary_op_greater, ">")
EZ_FLOW_BINARY_OP_IMPL(binary_op_greater, Boolean, Integer, Integer, >)
EZ_FLOW_BINARY_OP_IMPL(binary_op_greater, Boolean, Integer, Real, >)

EZ_FLOW_BINARY_OP_NOT_SUPPORTED(Integer, binary_op_greater_eq, ">=")
EZ_FLOW_BINARY_OP_IMPL(binary_op_greater_eq, Boolean, Integer, Integer, >=)
EZ_FLOW_BINARY_OP_IMPL(binary_op_greater_eq, Boolean, Integer, Real, >=)

EZ_FLOW_BINARY_OP_NOT_SUPPORTED(Integer, binary_op_eq, "==")
EZ_FLOW_BINARY_OP_IMPL(binary_op_eq, Boolean, Integer, Integer, ==)
EZ_FLOW_BINARY_OP_IMPL(binary_op_eq, Boolean, Integer, Real, ==)

EZ_FLOW_BINARY_OP_NOT_SUPPORTED(Integer, binary_op_not_eq, "!=")
EZ_FLOW_BINARY_OP_IMPL(binary_op_not_eq, Boolean, Integer, Integer, !=)
EZ_FLOW_BINARY_OP_IMPL(binary_op_not_eq, Boolean, Integer, Real, !=)

EZ_FLOW_TYPE_IMPL(Integer)
{
    Type result;
    result.id = result.name = "integer";
    result.construct.call = entity::make_constructor<Integer>(0);
    result.representation.call = [](const Entity& var) -> std::string {
        return std::to_string(var.as<Integer>().value());
    };

    result.assign.call = [](Entity& lhs, const Entity& rhs) -> EvalResult {
        Integer& self = lhs.as<Integer>();
        return rhs.match(
            [&](const Integer& rhs) -> EvalResult {
                self.value() = rhs.value();
                return Ok{lhs};
            },
            [&](const Real& rhs) -> EvalResult {
                self.value() = rhs.value();
                return Ok{lhs};
            },
            [&](const auto& rhs) -> EvalResult {
                return error::invalid_assignment(lhs.type().name, rhs.type().name);
            });
    };

    result.unary_op_plus.call = EZ_FLOW_UNARY_OP(Integer, unary_op_plus);
    result.unary_op_minus.call = EZ_FLOW_UNARY_OP(Integer, unary_op_minus);

    result.binary_op_mult.call = EZ_FLOW_BINARY_OP(Integer, binary_op_mult);
    result.binary_op_div.call = EZ_FLOW_BINARY_OP(Integer, binary_op_div);
    result.binary_op_plus.call = EZ_FLOW_BINARY_OP(Integer, binary_op_plus);
    result.binary_op_minus.call = EZ_FLOW_BINARY_OP(Integer, binary_op_minus);
    result.binary_op_less.call = EZ_FLOW_BINARY_OP(Integer, binary_op_less);
    result.binary_op_less_eq.call = EZ_FLOW_BINARY_OP(Integer, binary_op_less_eq);
    result.binary_op_greater_eq.call = EZ_FLOW_BINARY_OP(Integer, binary_op_greater_eq);
    result.binary_op_greater.call = EZ_FLOW_BINARY_OP(Integer, binary_op_greater);
    result.binary_op_eq.call = EZ_FLOW_BINARY_OP(Integer, binary_op_eq);
    result.binary_op_not_eq.call = EZ_FLOW_BINARY_OP(Integer, binary_op_not_eq);

    return result;
}

}  // namespace ez::flow::engine
