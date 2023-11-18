#include "types.hpp"

#include "../entity_utils.hpp"

namespace ez::flow::engine {

namespace array {

std::string representation(const Entity& var)
{
    std::string result = "[";

    uint index = 0;

    for (const Entity& val : var.as<Array>().value()) {
        result += entity::representation(val);
        if (++index != var.as<Array>().value().size()) result += ", ";
    }
    result += "]";

    return result;
}

EvalResult assign(Entity& lhs, const Entity& rhs)
{
    if (!same_type(lhs.type(), rhs.type())) {
        return error::invalid_assignment(lhs.type().name, rhs.type().name);
    }
    lhs.as<Array>().value() = rhs.as<Array>().value();
    return Ok{lhs};
}

Integer size(Array& self) { return self.value().size(); }

}  // namespace array

EZ_FLOW_TYPE_IMPL(Array)
{
    Type result;
    result.id = result.name = "array";
    result.construct.call = entity::make_constructor<Array>();
    result.representation.call = array::representation;
    result.assign.call = array::assign;
    result.static_properties = {EZ_FLOW_STATIC_PROPERTY(Array, Integer, "integer", "size") =
                                    array::size};
    return result;
}

}  // namespace ez::flow::engine
