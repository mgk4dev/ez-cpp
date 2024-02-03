#pragma once

#include <ez/rpc/messages.hpp>
#include <ez/rpc/transport.hpp>
#include <ez/rpc/types.hpp>

#include <ez/async/executor.hpp>
#include <ez/box.hpp>
#include <ez/trait.hpp>
#include <ez/tuple.hpp>

#include <boost/pfr.hpp>

namespace ez::rpc {

class AbstractFunction;

class AbstractRemoteService {
public:
    virtual ~AbstractRemoteService() = default;
    virtual async::IoContext& context() = 0;
    virtual AsyncResult<RequestId> invoke(std::string_view name_space,
                                          std::string_view fn_name,
                                          std::vector<ByteArray> arguments,
                                          Reply* reply) = 0;

    virtual void set_response_callback(const RequestId& request_id, std::function<void()> wake) = 0;
};

struct RemoteServiceBaseImpl;

struct RemoteServiceOptions {
    std::chrono::milliseconds poll_interval = std::chrono::milliseconds{100};
};

class RemoteServiceBase {
public:
    RemoteServiceBase(async::IoContext& context, Box<transport::Client> client);

    ~RemoteServiceBase();

    Result<void, std::runtime_error> connect_to(const std::string& server);

    void start(RemoteServiceOptions = {});

protected:
    void setup(AbstractFunction&);
    void setup(AbstractFunction&, std::string_view);

protected:
    Box<RemoteServiceBaseImpl> m_impl;
};

template <typename... Schemas>
class RemoteService : public RemoteServiceBase {
public:
    RemoteService(async::IoContext& context, Box<transport::Client> client)
        : RemoteServiceBase{context, std::move(client)}
    {
        m_schemas->for_each([&]<typename T>(T& schema) {
            if constexpr (requires {
                              {
                                  T::name_space
                              };
                          }) {
                boost::pfr::for_each_field(schema, [&](auto& f) { setup(f, T::name_space); });
            }
            else {
                boost::pfr::for_each_field(schema, [&](auto& f) { setup(f); });
            }
        });
    }

    auto* operator->() { return &m_schemas.value()[EZ_CONSTEXP(0)]; }

    template <typename T>
    auto& schema()
    {
        return std::get<T>(m_schemas.value());
    }

private:
    Box<Tuple<Schemas...>> m_schemas;
};
}  // namespace ez::rpc
