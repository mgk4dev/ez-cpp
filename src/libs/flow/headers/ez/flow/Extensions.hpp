#pragma once

#include <ez/Compiler.hpp>
#include <ez/async/Task.hpp>

EZ_PUSH_WARNING
EZ_GCC_DISABLE_WARNING("-Wstringop-overflow")
EZ_GCC_DISABLE_WARNING("-Warray-bounds")

#include <boost/json/object.hpp>

EZ_POP_WARNING

#include <functional>
#include <stdexcept>

namespace ez::flow::ext {
namespace detail {
template <typename... T>
struct DecomposeSignature;

template <typename R_, typename Arg_>
struct DecomposeSignature<R_(Arg_)> {
    using Arg = Arg_;
    using R = R_;
};

}  // namespace detail

using JsonObject = boost::json::object;

template <typename T>
using AsyncTask = async::Task<T>;

// clang-format off
template <typename Signature>
using AsyncRequest = std::function<
        AsyncTask<typename detail::DecomposeSignature<Signature>::R>
        (typename detail::DecomposeSignature<Signature>::Arg)
    >;
// clang-format on

using ActionRequest = AsyncRequest<JsonObject(JsonObject)>;
using CampaignRequest = AsyncRequest<JsonObject(JsonObject)>;
using DeviceInfoRequest = AsyncRequest<JsonObject(JsonObject)>;
using HttpRequest = AsyncRequest<JsonObject(JsonObject)>;

}  // namespace ez::flow::ext
