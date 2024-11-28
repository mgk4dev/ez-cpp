#include "Types.hpp"

#include "../builtin/Dictionary.hpp"

namespace ez::flow::engine {
EZ_FLOW_TYPE_IMPL(DeviceInfoRequest)
{
    return DictionaryType<DeviceInfoRequest,
                          [] { return "device_info_request"; }>::construct_type();
}

EZ_FLOW_TYPE_IMPL(DeviceInfoReply)
{
    return DictionaryType<DeviceInfoReply, [] { return "device_info_reply"; }>::construct_type();
}

}  // namespace ez::flow::engine
