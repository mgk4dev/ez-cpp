#pragma once

#include "../core/value_interface.hpp"

namespace ez::flow::engine {

struct Action : Value<> {
    EZ_FLOW_TYPE(EntityCategory::Object)
};

struct ActionReply : Value<> {
    EZ_FLOW_TYPE(EntityCategory::Object)
};

struct ApiCall : Value<> {
    EZ_FLOW_TYPE(EntityCategory::Object)
};

struct ApiCallReply : Value<> {
    EZ_FLOW_TYPE(EntityCategory::Object)
};

struct Campaign : Value<> {
    EZ_FLOW_TYPE(EntityCategory::Object)
};

struct CampaignReply : Value<> {
    EZ_FLOW_TYPE(EntityCategory::Object)
};

struct DeviceInfo : Value<> {
    EZ_FLOW_TYPE(EntityCategory::Object)
};

struct DeviceInfoReply : Value<> {
    EZ_FLOW_TYPE(EntityCategory::Object)
};

}  // namespace ez::flow::engine
