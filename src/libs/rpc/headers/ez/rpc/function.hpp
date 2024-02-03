#pragma once

#include <ez/rpc/function_utils.hpp>

#include <functional>

namespace ez::rpc {

class AbstractRemoteService;

class AbstractFunction {
public:
    virtual std::string_view name() const = 0;
    virtual AsyncResult<Reply> invoke(std::span<const ByteArray>) = 0;

    void set_client(AbstractRemoteService* client);
    const std::string& name_space() const;
    void set_name_space(std::string_view name_space);

protected:
    AsyncResult<Reply> invoke_remote(std::string_view name_space,
                                     std::string_view function_name,
                                     std::vector<ByteArray> args);

private:
    AbstractRemoteService* m_client = nullptr;
    std::string m_name_space;
};

template <typename...>
class Function;

template <typename R, typename... Args>
class Function<R(Args...)> : public AbstractFunction {
public:
    using ReturnType = AsyncResult<R>;
    using AsyncImplemenationType = std::function<async::Task<R>(Args...)>;
    using SyncImplemenationType = std::function<R(Args...)>;

    Function(std::string_view name) : m_name{name.begin(), name.end()} {}

    std::string_view name() const override { return m_name; }

    Function& operator=(AsyncImplemenationType&& impl)
    {
        m_impl = std::move(impl);
        return *this;
    }

    Function& operator=(SyncImplemenationType&& impl)
    {
        m_impl = [impl = std::move(impl)](Args... args) mutable -> async::Task<R> {
            co_return impl(std::move(args)...);
        };
        return *this;
    }

    // server side
    AsyncResult<Reply> invoke(std::span<const ByteArray> args) override
    {
        auto arg_tuple = func::extract_args<Args...>(args);

        if (!arg_tuple) co_return Fail{Error::remote_error(arg_tuple.error().what())};

        auto result = co_await std::apply(m_impl, std::move(arg_tuple.value()));

        Reply reply;
        reply.result = Serializer<R>::serialize(result);
        reply.type = ReplyType::Value;
        co_return Ok{std::move(reply)};
    }

    // client side
    ReturnType operator()(Args... args)
    {
        auto arg_array = func::serialize_args(args...);

        auto result = co_await invoke_remote(name_space(), m_name, std::move(arg_array));
        if (!result) co_return result.wrapped_error();

        co_return func::get_return_value<R>(std::move(result.value()));
    }

private:
    std::string m_name;
    AsyncImplemenationType m_impl;
};

}  // namespace ez::rpc

#define EZ_RPC_NAMESAPCE(name) static inline constexpr std::string_view name_space = EZ_STR(name)
