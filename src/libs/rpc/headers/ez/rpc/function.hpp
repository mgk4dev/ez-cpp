#pragma once

#include <ez/rpc/function_utils.hpp>

#include <functional>
#include <span>

namespace ez::rpc {

class AbstractRemoteService;

class AbstractFunction {
public:
    virtual std::string_view name() const = 0;
    virtual AsyncResult<RawReply> invoke(std::span<const ByteArray>) = 0;

    void set_client(AbstractRemoteService* client);
    const std::string& name_space() const;
    void set_name_space(std::string_view name_space);

protected:
    AsyncResult<RawReply> invoke_remote(std::string_view name_space,
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
    using AsyncImplemenationType = std::function<async::Task<R>(arg::in<Args>...)>;
    using SyncImplemenationType = std::function<R(arg::in<Args>...)>;

    Function(std::string_view name) : m_name{name.begin(), name.end()} {}

    std::string_view name() const override { return m_name; }

    Function& operator=(AsyncImplemenationType&& impl)
    {
        m_impl = std::move(impl);
        return *this;
    }

    Function& operator=(SyncImplemenationType&& impl)
    {
        m_impl = [impl = std::move(impl)](arg::in<Args>... args) mutable -> async::Task<R> {
            co_return impl(args...);
        };
        return *this;
    }

    void bind_to(Function impl)
    {
        m_impl = [impl = std::move(impl)](arg::in<Args>... args) mutable -> async::Task<R> {
            auto result = co_await impl(args...);
            if (result) { co_return result.value(); }
            throw result.error();
        };
    }

    // server side
    AsyncResult<RawReply> invoke(std::span<const ByteArray> args) override
    {
        try {
            auto arg_tuple = func::extract_args<Args...>(args);

            if (!arg_tuple) co_return Fail{Error::internal_error(arg_tuple.error().what())};

            auto result = co_await std::apply(m_impl, std::move(arg_tuple.value()));

            RawReply reply = Ok{std::move(serialize(result))};
            co_return Ok{std::move(reply)};
        }
        catch (const Error& error) {
            co_return Fail{std::move(error)};
        }
    }

    // client side
    ReturnType operator()(arg::in<Args>... args)
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
