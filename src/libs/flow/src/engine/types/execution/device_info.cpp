#include "types.hpp"

#include "../entity_utils.hpp"

namespace ez::flow::engine {

EZ_FLOW_TYPE_IMPL(DeviceInfo)
{
    Type result;
    result.id = result.name = "device_info";
    result.construct.call = [](Entity) -> EvalResult { return Ok{DeviceInfo{}}; };
    return result;
}

EZ_FLOW_TYPE_IMPL(DeviceInfoReply)
{
    Type result;
    result.id = result.name = "device_info_reply";
    result.construct.call = [](Entity) -> EvalResult { return Ok{DeviceInfoReply{}}; };
    return result;
}

}  // namespace ez::flow::engine
