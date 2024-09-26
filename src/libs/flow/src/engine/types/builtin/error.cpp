#include "types.hpp"

#include "../entity_utils.hpp"

namespace ez::flow::engine {
EZ_FLOW_TYPE_IMPL(Error)
{
    Type result;
    result.id = result.name = "error";
    result.representation.call = [](const Entity& var) -> std::string {
        return var.as<Error>().what;
    };
    return result;
}

}  // namespace ez::flow::engine
