#include <ez/rpc/Function.hpp>

#include <ez/rpc/RemoteService.hpp>

#include <ez/async/Operation.hpp>

namespace ez::rpc {
void AbstractFunction::set_client(AbstractRemoteService* client) { m_client = client; }

const std::string& AbstractFunction::name_space() const { return m_name_space; }

void AbstractFunction::set_name_space(std::string_view name_space) { m_name_space = name_space; }

struct WaitForResponse {
    AbstractRemoteService& client;
    const RequestId& request_id;

    void start(auto&) {}

    bool done() const { return false; }

    void start(auto continuation)
    {
        client.set_response_callback(request_id, [continuation]() mutable { continuation(); });
    }

    void cancel() {}

    Unit result() { return {}; }
};

AsyncResult<RawReply> AbstractFunction::invoke_remote(std::string_view name_space,
                                                      std::string_view function_name,
                                                      std::vector<ByteArray> args)
{
    RawReply reply;
    auto id = co_await m_client->invoke(name_space, function_name, std::move(args), &reply);
    if (!id) co_return Fail{id.error()};

    co_await async::Operation<WaitForResponse>{*m_client, id.value()};
    co_return std::move(reply);
}

}  // namespace ez::rpc
