#include <ez/flow/engine.hpp>

#include <ez/flow/parser.hpp>

#include "eval/eval.hpp"

#include <boost/spirit/home/x3/support/utility/error_reporting.hpp>

#include <sstream>

namespace ez::flow {

struct Engine::Impl {};

Engine::Engine() {}

void Engine::set_logger(Logger logger) { m_logger = std::move(logger); }

Engine::~Engine() = default;

void Engine::eval(std::string_view code, std::string_view file_name)
{
    try {
        auto program_result = flow::parse(code, file_name);

        if (!program_result) {
            m_logger.error("Failed to parse program:\n{}", program_result.error_message);
            return;
        }

        engine::EvaluationContext context;
        context.logger = m_logger;
        engine::eval(context, program_result);
    }
    catch (const engine::EvaluationError& error) {
        using ErrorHandler = boost::spirit::x3::error_handler<std::string_view::iterator>;

        std::stringstream error_string_stream;
        ErrorHandler error_handler{code.begin(), code.end(), error_string_stream,
                                   std::string{file_name.begin(), file_name.end()}};

        auto error_message_header = std::format("Evaluation eror: {}", error.what());

        error_handler(code.begin() + error.location.start_position, error_message_header);

        m_logger.error("{}", error_string_stream.str());
    }
    catch (const std::exception& error) {
        m_logger.error("Internal error: {}", error.what());
    }
}

void Engine::set_action_delegate(ext::ActionRequest delegate)
{
    m_run_action_delegate = std::move(delegate);
}

void Engine::set_campaign_delegate(ext::CampaignRequest delegate)
{
    m_run_campaign_delegate = std::move(delegate);
}

void Engine::set_device_info_delegate(ext::DeviceInfoRequest delegate)
{
    m_request_device_info_delegate = std::move(delegate);
}

void Engine::set_api_call_delegate(ext::ApiCallRequest delegate)
{
    m_call_api_delegate = std::move(delegate);
}

}  // namespace ez::flow
