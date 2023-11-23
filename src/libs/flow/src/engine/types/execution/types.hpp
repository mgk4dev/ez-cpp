#pragma once

#include "../builtin/types.hpp"
#include "../core/value_interface.hpp"

namespace ez::flow::engine {

struct Action : Dictionary {
    EZ_FLOW_TYPE(EntityCategory::Object)
    using Dictionary::operator=;
};

struct ActionReply : Dictionary {
    EZ_FLOW_TYPE(EntityCategory::Object)
    using Dictionary::operator=;
};

struct HttpRequest : Dictionary {
    EZ_FLOW_TYPE(EntityCategory::Object)
    using Dictionary::operator=;
};

struct HttpReply : Dictionary {
    EZ_FLOW_TYPE(EntityCategory::Object)
    using Dictionary::operator=;
};

struct Campaign : Dictionary {
    EZ_FLOW_TYPE(EntityCategory::Object)
    using Dictionary::Dictionary;
    using Dictionary::operator=;
};

struct CampaignReply : Dictionary {
    EZ_FLOW_TYPE(EntityCategory::Object)
    using Dictionary::operator=;
};

struct DeviceInfoRequest : Dictionary {
    EZ_FLOW_TYPE(EntityCategory::Object)
    using Dictionary::operator=;
};

struct DeviceInfoReply : Dictionary {
    EZ_FLOW_TYPE(EntityCategory::Object)
    using Dictionary::operator=;
};

}  // namespace ez::flow::engine
