#include "types.hpp"

#include "../entity_utils.hpp"

namespace ez::flow::engine {

EZ_FLOW_TYPE_IMPL(ApiCall)
{
    Type result;
    result.id = result.name = "api_call";
    result.construct.call = [](Entity) -> EvalResult { return Ok{ApiCall{}}; };
    return result;
}

EZ_FLOW_TYPE_IMPL(ApiCallReply)
{
    Type result;
    result.id = result.name = "api_call_reply";
    result.construct.call = [](Entity) -> EvalResult { return Ok{ApiCallReply{}}; };
    return result;
}

}  // namespace ez::flow::engine
