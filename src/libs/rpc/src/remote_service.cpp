#include <ez/rpc/remote_service.hpp>

#include <ez/rpc/function.hpp>
#include <ez/rpc/serializer.hpp>

#include "protobuf/messages.pb.h"

#include <boost/asio/steady_timer.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>

namespace ez::rpc {

RequestId make_request_id()
{
    return RequestId{boost::uuids::to_string(boost::uuids::random_generator()())};
}

struct RemoteServiceBaseImpl : public AbstractRemoteService {
    struct Query {
        RawReply* reply;
        std::function<void()> wake;
    };

    Ref<async::IoContext> context_;
    Box<transport::Client> transport;
    std::unordered_map<RequestId, Query> queries;
    boost::asio::steady_timer poll_timer;
    RemoteServiceOptions options;

    RemoteServiceBaseImpl(async::IoContext& context, Box<transport::Client> client)
        : context_{context}, transport{std::move(client)}, poll_timer{context}
    {
    }

    async::IoContext& context() override { return context_.get(); }

    AsyncResult<RequestId> invoke(std::string_view name_space,
                                  std::string_view fn_name,
                                  std::vector<ByteArray> arguments,
                                  RawReply* reply) override
    {
        auto id = make_request_id();

        auto& query = queries[id];
        query.reply = reply;
        protobuf::Request request;
        request.set_id(id.value());
        *request.mutable_name_space() = name_space;
        *request.mutable_function_name() = fn_name;
        for (auto& arg : arguments) *request.add_arguments() = arg.value();

        auto ok = co_await transport->send(serialize(request));
        if (!ok) co_return ok.wrapped_error();
        co_return Ok{std::move(id)};
    }

    void set_response_callback(const RequestId& request_id, std::function<void()> wake) override
    {
        auto& query = queries[request_id];
        query.wake = std::move(wake);
    }

    void poll()
    {
        ByteArray data;

        while (transport->receive(data)) {
            auto reply_result = Serializer<protobuf::Reply>::deserialize(data);
            if (!reply_result) {
                // TODO log error
                continue;
            }

            protobuf::Reply& reply = reply_result.value();

            const auto request_id = RequestId{reply.request_id()};

            if (!queries.contains(request_id)) continue;

            auto& query = queries[request_id];

            if (reply.has_value()) { *query.reply = Ok{ByteArray{reply.value()}}; }
            else {
                *query.reply = Fail{Error{Error::Code(reply.error().code()), reply.error().what()}};
            }

            async::post(context_.get(), query.wake);
            queries.erase(request_id);
        }

        start();
    }

    void start()
    {
        poll_timer.expires_from_now(std::chrono::milliseconds{options.poll_interval});
        poll_timer.async_wait([this](boost::system::error_code error) {
            if (!error) poll();
        });
    }
};

RemoteServiceBase::RemoteServiceBase(async::IoContext& context, Box<transport::Client> client)
    : m_impl{in_place, context, std::move(client)}
{
}

RemoteServiceBase::~RemoteServiceBase() {}

Result<void, std::runtime_error> RemoteServiceBase::connect_to(const std::string& server)
{
    return m_impl->transport->connect(server);
}

void RemoteServiceBase::start(RemoteServiceOptions options)
{
    m_impl->options = options;
    m_impl->start();
}

void RemoteServiceBase::setup(AbstractFunction& f) { f.set_client(&m_impl.value()); }

void RemoteServiceBase::setup(AbstractFunction& f, std::string_view name_space)
{
    setup(f);
    f.set_name_space(name_space);
}

}  // namespace ez::rpc
