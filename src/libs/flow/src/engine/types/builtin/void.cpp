#include "types.hpp"

#include "../entity_utils.hpp"

namespace ez::flow::engine {
EZ_FLOW_TYPE_IMPL(Void)
{
    Type result;
    result.id = result.name = "void";
    result.construct.call = entity::make_constructor<Void>();
    result.representation.call = [](const Entity&) -> std::string { return "void"; };
    return result;
}

}  // namespace ez::flow::engine
