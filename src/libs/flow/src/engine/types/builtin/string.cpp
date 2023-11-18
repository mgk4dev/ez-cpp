#include "types.hpp"

#include "../entity_utils.hpp"

namespace ez::flow::engine {

EZ_FLOW_TYPE_IMPL(String)
{
    Type result;
    result.id = result.name = "string";
    result.construct.call = entity::make_constructor<String>();
    result.representation.call = [](const Entity& var) -> std::string {
        return std::format("{}", var.as<String>().value());
    };

    return result;
}

}  // namespace ez::flow::engine
