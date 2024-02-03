#include <ez/rpc/function.hpp>

#include <ez/rpc/remote_service.hpp>

#include <ez/async/operation.hpp>

namespace ez::rpc {

void AbstractFunction::set_client(AbstractRemoteService* client) { m_client = client; }

const std::string& AbstractFunction::name_space() const { return m_name_space; }

void AbstractFunction::set_name_space(std::string_view name_space) { m_name_space = name_space; }

struct WaitForResponse {
    AbstractRemoteService& client;
    const RequestId& request_id;

    void start(auto&) {}

    bool is_ready() const { return false; }

    void on_done(auto continuation)
    {
        client.set_response_callback(request_id, [continuation]() mutable { continuation(); });
    }

    void cancel() {}

    Unit result() { return {}; }
};

AsyncResult<Reply> AbstractFunction::invoke_remote(std::string_view name_space,
                                                   std::string_view function_name,
                                                   std::vector<ByteArray> args)
{
    Reply reply;
    auto id = co_await m_client->invoke(name_space, function_name, std::move(args), &reply);
    if (!id) co_return id.wrapped_error();

    co_await async::ContextFreeOperation<WaitForResponse>{*m_client, id.value()};
    co_return Ok{std::move(reply)};
}

}  // namespace ez::rpc
