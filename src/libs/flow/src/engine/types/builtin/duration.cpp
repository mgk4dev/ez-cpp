#include "types.hpp"

#include "../entity_utils.hpp"

namespace ez::flow::engine {

EZ_FLOW_TYPE_IMPL(Duration)
{
    Type result;
    result.id = result.name = "duration";
    result.construct.call = entity::make_constructor<Duration>(in_place, 0, ast::DurationUnit::Sec);
    return result;
}

}  // namespace ez::flow::engine
