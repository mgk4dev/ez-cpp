#pragma once

#include <ez/rpc/Serializer.hpp>

#include <ez/Option.hpp>
#include <ez/Tuple.hpp>

#include <span>

namespace ez::rpc::func {
template <typename... Args>
Result<Tuple<Args...>, ParsingError> extract_args(std::span<const ByteArray> args)
{
    Tuple<Args...> result;
    Option<ParsingError> error;
    for_constexpr<0, result.size()>([&](auto index) {
        if (error) return;

        using T = std::tuple_element_t<index, Tuple<Args...>>;
        auto val = deserialize<T>(args[index]);
        if (val)
            result[index] = val.value();
        else
            error = val.error();
    });

    if (error) return Fail{std::move(error.value())};

    return Ok{std::move(result)};
}

template <typename... Args>
std::vector<ByteArray> serialize_args(const Args&... args_)
{
    std::vector<ByteArray> result;
    Tuple args = std::forward_as_tuple(args_...);
    args.for_each([&](auto&& arg) { result.push_back(serialize(arg)); });
    return result;
}

template <typename R>
Result<R, Error> get_return_value(RawReply reply)
{
    if (reply) { return Ok{deserialize<R>(reply.value()).value()}; }
    else {
        return Fail{reply.error()};
    }
}

}  // namespace ez::rpc::func
