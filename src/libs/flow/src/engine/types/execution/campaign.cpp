#include "types.hpp"

#include "../entity_utils.hpp"

namespace ez::flow::engine {

EZ_FLOW_TYPE_IMPL(Campaign)
{
    Type result;
    result.id = result.name = "campaign";
    result.construct.call = [](Entity) -> EvalResult { return Ok{Campaign{}}; };
    return result;
}

EZ_FLOW_TYPE_IMPL(CampaignReply)
{
    Type result;
    result.id = result.name = "campaign_reply";
    result.construct.call = [](Entity) -> EvalResult { return Ok{CampaignReply{}}; };
    return result;
}

}  // namespace ez::flow::engine
