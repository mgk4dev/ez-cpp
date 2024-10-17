#pragma once

#include <ez/flow/extensions.hpp>
#include <ez/flow/program.hpp>

#include <ez/async/Executor.hpp>

#include <ez/Box.hpp>
#include <ez/Logger.hpp>

#include <boost/asio.hpp>

namespace ez::flow {

using IoContext = boost::asio::io_context;

class Engine {
public:
    Engine(IoContext&);
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

namespace ez::async {
template <>
struct Executor<flow::IoContext> {
    static void post(flow::IoContext& context, auto&& f) { boost::asio::post(context, EZ_FWD(f)); }
};

}  // namespace ez::async
