#pragma once

#include <ez/meta.hpp>

#include "builtin/functions.hpp"
#include "builtin/types.hpp"
#include "core/function.hpp"
#include "core/type.hpp"
#include "core/workflow.hpp"
#include "execution/types.hpp"

namespace ez::flow::engine {

// clang-format off
using EntityTypeList = TypeList <
    Error,

    Void,
    Boolean,
    Integer,
    Real,
    String,
    Duration,

    Array,
    Dictionary,
    MemberFunction,

    FreeFunction,
    WorkflowDefinition,
    WorkflowInvocation,

    Action,
    ActionReply,
    Campaign,
    CampaignReply,
    ApiCall,
    ApiCallReply,
    DeviceInfo,
    DeviceInfoReply
    > ;
// clang-format on

}  // namespace ez::flow::engine
