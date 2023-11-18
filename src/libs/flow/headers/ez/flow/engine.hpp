#pragma once

#include <ez/flow/extensions.hpp>
#include <ez/flow/program.hpp>

#include <ez/box.hpp>
#include <ez/reporting.hpp>

namespace ez::flow {

class Engine {
public:
    Engine();
    ~Engine();

    void set_logger(Logger logger);

    void eval(std::string_view code, std::string_view file_name = {});

    void set_action_delegate(ext::ActionRequest delegate);
    void set_campaign_delegate(ext::CampaignRequest delegate);
    void set_device_info_delegate(ext::DeviceInfoRequest delegate);
    void set_api_call_delegate(ext::ApiCallRequest delegate);

private:
    struct Impl;
    Box<Impl> m_impl;
    Logger m_logger;
    ext::ActionRequest m_run_action_delegate;
    ext::CampaignRequest m_run_campaign_delegate;
    ext::DeviceInfoRequest m_request_device_info_delegate;
    ext::ApiCallRequest m_call_api_delegate;
};

}  // namespace ez::flow
