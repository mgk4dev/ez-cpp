#pragma once

#include <ez/flow/extensions.hpp>
#include <ez/flow/program.hpp>

#include <ez/async/Executor.hpp>
#include <ez/io/Context.hpp>

#include <ez/Box.hpp>
#include <ez/Logger.hpp>

namespace ez::flow {

class Engine {
public:
    Engine(io::Context&);
    ~Engine();

    void set_logger(Logger logger);

    void eval(std::string code, std::string file_name, unsigned int id);

    void set_action_delegate(ext::ActionRequest delegate);
    void set_campaign_delegate(ext::CampaignRequest delegate);
    void set_device_info_delegate(ext::DeviceInfoRequest delegate);
    void set_http_request_delegate(ext::HttpRequest delegate);

private:
    struct Impl;
    Box<Impl> m_impl;
};

}  // namespace ez::flow
