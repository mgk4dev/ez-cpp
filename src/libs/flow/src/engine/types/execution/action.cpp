#include "types.hpp"

#include "../entity_utils.hpp"

namespace ez::flow::engine {

EZ_FLOW_TYPE_IMPL(Action)
{
    Type result;
    result.id = result.name = "action";
    result.construct.call = [](Entity) -> EvalResult { return Ok{Action{}}; };
    return result;
}

EZ_FLOW_TYPE_IMPL(ActionReply)
{
    Type result;
    result.id = result.name = "action_reply";
    result.construct.call = [](Entity) -> EvalResult { return Ok{ActionReply{}}; };
    return result;
}

}  // namespace ez::flow::engine
