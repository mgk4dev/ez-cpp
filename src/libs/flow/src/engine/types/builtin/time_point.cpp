#include "types.hpp"

#include "../entity_utils.hpp"

namespace ez::flow::engine {
TimePoint TimePoint::now()
{
    TimePoint tp;
    tp.value = std::chrono::system_clock::now();
    return tp;
}

EZ_FLOW_TYPE_IMPL(TimePoint)
{
    Type result;
    result.id = result.name = "time_point";
    result.construct.call = entity::make_constructor<TimePoint>(now());
    return result;
}

}  // namespace ez::flow::engine
