#pragma once

#include <ez/TypeUtils.hpp>

#include "builtin/Functions.hpp"
#include "builtin/Types.hpp"
#include "core/Function.hpp"
#include "core/Type.hpp"
#include "core/Workflow.hpp"
#include "execution/Types.hpp"

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
    TimePoint,

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
    HttpRequest,
    HttpReply,
    DeviceInfoRequest,
    DeviceInfoReply
    > ;
// clang-format on

}  // namespace ez::flow::engine
