#include "Types.hpp"

#include "../EntityUtils.hpp"

namespace ez::flow::engine {
EZ_FLOW_TYPE_IMPL(Real)
{
    Type result;
    result.id = result.name = "real";
    result.construct.call = entity::make_constructor<Real>(in_place, 0.0);
    result.representation.call = [](const Entity& var) -> std::string {
        return std::to_string(var.as<Real>().value());
    };

    return result;
}

}  // namespace ez::flow::engine
