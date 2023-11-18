#include "types.hpp"

#include "../entity_utils.hpp"

namespace ez::flow::engine {

EZ_FLOW_TYPE_IMPL(Boolean)
{
    Type result;
    result.id = result.name = "boolean";
    result.construct.call = entity::make_constructor<Array>(in_place, false);
    result.representation.call = [](const Entity& var) -> std::string {
        if (var.as<Boolean>().value()) return "true";
        return "false";
    };

    return result;
}

}  // namespace ez::flow::engine
