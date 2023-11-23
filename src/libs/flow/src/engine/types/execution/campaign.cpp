#include "types.hpp"

#include "../builtin/dictionary_type.hpp"

namespace ez::flow::engine {

EZ_FLOW_TYPE_IMPL(Campaign)
{
    return DictionaryType<Campaign, [] { return "campaign"; }>::construct_type();
}

EZ_FLOW_TYPE_IMPL(CampaignReply)
{
    return DictionaryType<CampaignReply, [] { return "campaign_reply"; }>::construct_type();
}

}  // namespace ez::flow::engine
