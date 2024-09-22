#include <ez/rpc/Service.hpp>

#include <ez/rpc/Function.hpp>
#include <ez/rpc/Serializer.hpp>

#include "protobuf/messages.pb.h"

namespace ez::rpc {

void AbstractService::poll()
{
    Message message;

    while (transport->receive(message)) {
        auto request_result = deserialize<protobuf::Request>(message.payload);
        if (!request_result) continue;

        protobuf::Request request = std::move(request_result).value();

        const auto& request_id = request.id();
        const auto& name_space = request.name_space();
        const auto& fn_name = request.function_name();
        const auto& peer_id = message.peer_id;

        AbstractFunction* f = find_function(name_space, fn_name);

        if (!f) {
            protobuf::Error error;
            error.set_code(protobuf::Error_Code_FunctionNotFound);
            error.set_what(std::format("Function {}.{} not found", name_space, fn_name));

            protobuf::Reply reply;
            reply.set_request_id(request_id);
            *reply.mutable_error() = std::move(error);

            task_pool << [this](PeerId peer_id, protobuf::Reply reply) -> async::Task<> {
                auto ok = co_await send(std::move(peer_id), std::move(reply));
                // TODO log error
                unused(ok);
            }(peer_id, std::move(reply));
        }

        std::vector<ByteArray> args;

        for (auto& arg : request.arguments()) args.push_back(ByteArray{arg});

        task_pool << exec(f, std::move(args), peer_id, RequestId{request_id});
    }

    start();
}

async::Task<> AbstractService::exec(AbstractFunction* f,
                                    std::vector<ByteArray> args,
                                    PeerId peer_id,
                                    RequestId request_id)
{
    Result<RawReply, Error> invoke_result = co_await f->invoke(args);

    if (!invoke_result) {
        auto ok = co_await send_error(invoke_result.error(), peer_id, request_id);
        // TODO log error
        unused(ok);
    }

    RawReply& invoke_return = invoke_result.value();

    if (!invoke_return) {
        auto ok = co_await send_error(invoke_return.error(), peer_id, request_id);
        // TODO log error
        unused(ok);
    }

    protobuf::Reply reply;
    reply.set_request_id(request_id.value());
    reply.set_value(invoke_return.value().value());

    auto ok = co_await send(peer_id, reply);
    // TODO log error
    unused(ok);
    co_return;
}

AsyncResult<> AbstractService::send_error(const Error& e, PeerId peer_id, RequestId request_id)
{
    protobuf::Reply reply;
    reply.set_request_id(request_id.value());

    protobuf::Error error;
    error.set_code(protobuf::Error_Code(e.code));
    error.set_what(e.what);

    *reply.mutable_error() = std::move(error);

    co_return co_await transport->send(peer_id, serialize(reply));
}

AsyncResult<> AbstractService::send(PeerId peer_id, protobuf::Reply reply)
{
    co_return co_await transport->send(peer_id, serialize(reply));
}

void AbstractService::start()
{
    poll_timer.expires_from_now(options.poll_interval);
    poll_timer.async_wait([this](boost::system::error_code error) {
        if (!error) poll();
    });
}

}  // namespace ez::rpc
