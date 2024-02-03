#include <ez/rpc/service.hpp>

#include <ez/rpc/function.hpp>
#include <ez/rpc/serializer.hpp>

#include <ranges>

namespace ez::rpc {

void AbstractService::poll()
{
    Message message;

    while (transport->receive(message)) {
        auto request_result = deserialize<Request>(message.payload);
        if (!request_result) continue;

        Request request = std::move(request_result).value();

        const auto& request_id = request.request_id;
        const auto& name_space = request.name_space;
        const auto& fn_name = request.function_name;
        const auto& peer_id = message.peer_id;

        AbstractFunction* f = find_function(name_space, fn_name);

        if (!f) {
            Reply reply;

            reply.request_id = request_id;
            reply.type = ReplyType::Error;
            reply.result = serialize(Error::remote_error("Remote function not found"));

            auto send = [this](PeerId peer_id, Reply reply) -> async::Task<> {
                co_await transport->send(peer_id, serialize(reply));
            };
            task_pool.post(send(peer_id, std::move(reply)));
        }

        auto exec = [this](AbstractFunction* f, std::vector<ByteArray> args, PeerId peer_id,
                           RequestId request_id) -> async::Task<> {
            auto reply_result = co_await f->invoke(args);

            if (reply_result) {
                reply_result.value().request_id = request_id;

                auto ok = co_await transport->send(peer_id, serialize(reply_result.value()));

                unused(ok);
                co_return;
            }

            Reply reply;
            reply.type = ReplyType::Error;
            reply.result = serialize(Error::remote_error(reply_result.error().what()));

            auto ok = co_await transport->send(peer_id, serialize(reply));

            unused(ok);
        };

        task_pool.post(exec(f, std::move(request.arguments), peer_id, request_id));
    }

    start();
}

void AbstractService::start()
{
    poll_timer.expires_from_now(options.poll_interval);
    poll_timer.async_wait([this](boost::system::error_code error) {
        if (!error) poll();
    });
}

}  // namespace ez::rpc
