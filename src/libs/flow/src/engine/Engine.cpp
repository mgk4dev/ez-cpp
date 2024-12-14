#include <ez/flow/Engine.hpp>

#include "eval/Interpreter.hpp"

#include <ez/flow/Errors.hpp>
#include <ez/flow/Parser.hpp>

#include <ez/async/Scope.hpp>

#include <boost/spirit/home/x3/support/utility/error_reporting.hpp>

#include <sstream>

namespace ez::flow {
struct Engine::Impl {
    Ref<io::Context> io_context;
    async::Scope<io::Context> scope;

    Logger logger;
    ext::ActionRequest run_action_delegate;
    ext::CampaignRequest run_campaign_delegate;
    ext::DeviceInfoRequest request_device_info_delegate;
    ext::HttpRequest http_request_delegate;

    void configure_interpreter(engine::Interpreter& interpreter)
    {
        interpreter.extensions = {logger, run_action_delegate, run_campaign_delegate,
                                  request_device_info_delegate, http_request_delegate};
    }
};

Engine::Engine(io::Context& io_context) : m_impl{std::in_place, io_context, io_context} {}

void Engine::set_logger(Logger logger) { m_impl->logger = std::move(logger); }

Engine::~Engine() = default;

void Engine::eval(std::string code, std::string file_name, unsigned int id)
{
    auto eval_file = [&](io::Context& io_context, std::string code, std::string file_name,
                         unsigned int id) -> async::Task<> {
        try {
            auto program_result = flow::parse(code, file_name);

            if (!program_result) {
                m_impl->logger.error("Failed to parse program:\n{}", program_result.error_message);
                co_return;
            }

            engine::Interpreter interpreter{io_context};
            m_impl->configure_interpreter(interpreter);
            interpreter.set_instance_id(id);

            co_await interpreter.eval(program_result);
        }
        catch (const engine::EvaluationError& error) {
            using ErrorHandler = boost::spirit::x3::error_handler<std::string::iterator>;

            std::stringstream error_string_stream;
            ErrorHandler error_handler{code.begin(), code.end(), error_string_stream,
                                       std::string{file_name.begin(), file_name.end()}};

            auto error_message_header = std::format("Evaluation eror: {}", error.what());

            error_handler(code.begin() + error.location.start_position, error_message_header);

            m_impl->logger.error("{}", error_string_stream.str());
        }
        catch (const Panic& error) {
            m_impl->logger.error("Panic: {}", error.what());
        }
        catch (const std::exception& error) {
            m_impl->logger.error("Internal error: {}", error.what());
        }
    };

    m_impl->scope << eval_file(m_impl->io_context, std::move(code), std::move(file_name), id);
}

void Engine::set_action_delegate(ext::ActionRequest delegate)
{
    m_impl->run_action_delegate = std::move(delegate);
}

void Engine::set_campaign_delegate(ext::CampaignRequest delegate)
{
    m_impl->run_campaign_delegate = std::move(delegate);
}

void Engine::set_device_info_delegate(ext::DeviceInfoRequest delegate)
{
    m_impl->request_device_info_delegate = std::move(delegate);
}

void Engine::set_http_request_delegate(ext::HttpRequest delegate)
{
    m_impl->http_request_delegate = std::move(delegate);
}

}  // namespace ez::flow
