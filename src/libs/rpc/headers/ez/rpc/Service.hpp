#pragma once

#include <ez/rpc/Transport.hpp>

#include <ez/async/Executor.hpp>
#include <ez/async/Scope.hpp>

#include <ez/Box.hpp>
#include <ez/Tuple.hpp>
#include <ez/Utils.hpp>

#include <boost/asio/steady_timer.hpp>
#include <boost/pfr.hpp>

#include <queue>

namespace ez::rpc {
class AbstractFunction;

namespace protobuf {
class Reply;
}  // namespace protobuf

struct ServiceOptions {
    std::chrono::microseconds poll_interval = std::chrono::milliseconds{100};
};

class AbstractService {
public:
    using Message = transport::Server::Message;

    Ref<IoContext> context;
    Box<transport::Server> transport;
    boost::asio::steady_timer poll_timer;
    async::Scope<IoContext> scope;
    ServiceOptions options;

    AbstractService(IoContext& ctx, Box<transport::Server> server)
        : context{ctx}, transport{std::move(server)}, poll_timer{ctx}, scope{ctx}
    {
    }
    virtual ~AbstractService() = default;

    void poll();
    void start();

    virtual AbstractFunction* find_function(std::string_view name_space,
                                            std::string_view function_name) = 0;

    async::Task<> exec(AbstractFunction* f,
                       std::vector<ByteArray> args,
                       PeerId peer_id,
                       RequestId request_id);

    AsyncResult<> send_error(const Error& e, PeerId peer_id, RequestId request_id);

    AsyncResult<> send(PeerId peer_id, protobuf::Reply reply);
};

template <typename... Schemas>
class Service {
public:
    Service(IoContext& context, Box<transport::Server> server)
        : m_impl{in_place, context, std::move(server)}
    {
    }

    Result<void, std::runtime_error> bind_to(const std::string& address)
    {
        return m_impl->transport->bind_to(address);
    }

    auto& implementation() { return m_impl->schemas[EZ_CONSTEXP(0)]; }

    template <typename T>
    auto& implementation()
    {
        return std::get<T>(m_impl->schemas);
    }

    void start(ServiceOptions options = {})
    {
        m_impl->options = options;
        m_impl->start();
    }

private:
    struct ServiceImpl : public AbstractService {
        using AbstractService::AbstractService;

        Tuple<Schemas...> schemas;

        AbstractFunction* find_function(std::string_view name_space, std::string_view name) override
        {
            AbstractFunction* result = nullptr;

            schemas.for_each([&]<typename T>(T& schema) {
                if constexpr (requires { {T::name_space}; }) {
                    boost::pfr::for_each_field(schema, [&](auto& f) {
                        if (f.name() == name && name_space == schema.name_space) result = &f;
                    });
                }
                else {
                    boost::pfr::for_each_field(schema, [&](auto& f) {
                        if (f.name() == name && name_space.empty()) result = &f;
                    });
                }
            });

            return result;
        }
    };

    Box<ServiceImpl> m_impl;
};

}  // namespace ez::rpc
