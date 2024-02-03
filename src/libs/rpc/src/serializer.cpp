#include <ez/rpc/serializer.hpp>

#include <boost/json.hpp>
#include <boost/json/src.hpp>

namespace ez::rpc {

namespace json = boost::json;

#define EZ_RPC_NUMBER_SERIALIZER_IMPL(T)                                      \
    ByteArray Serializer<T>::serialize(T val)                                 \
    {                                                                         \
        return ByteArray{json::serialize(json::value(val))};                  \
    }                                                                         \
    Result<T, ParsingError> Serializer<T>::deserialize(const ByteArray& data) \
    {                                                                         \
        try {                                                                 \
            return Ok{json::parse(data.value()).to_number<T>()};              \
        }                                                                     \
        catch (const std::exception& e) {                                     \
            return Fail{e.what()};                                            \
        }                                                                     \
    }

#define EZ_RPC_SERIALIZER_IMPL(T, convert)                                    \
    ByteArray Serializer<T>::serialize(T val)                                 \
    {                                                                         \
        return ByteArray{json::serialize(json::value(val))};                  \
    }                                                                         \
    Result<T, ParsingError> Serializer<T>::deserialize(const ByteArray& data) \
    {                                                                         \
        try {                                                                 \
            return Ok{json::parse(data.value()).convert()};                   \
        }                                                                     \
        catch (const std::exception& e) {                                     \
            return Fail{e.what()};                                            \
        }                                                                     \
    }

///////////////////////////////////////////////////////////////////////////////

EZ_RPC_NUMBER_SERIALIZER_IMPL(std::uint16_t)
EZ_RPC_NUMBER_SERIALIZER_IMPL(std::uint32_t)
EZ_RPC_NUMBER_SERIALIZER_IMPL(std::uint64_t)

EZ_RPC_NUMBER_SERIALIZER_IMPL(std::int16_t)
EZ_RPC_NUMBER_SERIALIZER_IMPL(std::int32_t)
EZ_RPC_NUMBER_SERIALIZER_IMPL(std::int64_t)

EZ_RPC_NUMBER_SERIALIZER_IMPL(float)
EZ_RPC_NUMBER_SERIALIZER_IMPL(double)

EZ_RPC_SERIALIZER_IMPL(bool, as_bool)

//////////////////////////////////////////////////////////////////////////////

ByteArray Serializer<std::string>::serialize(const std::string& val)
{
    return ByteArray{json::serialize(json::value(val))};
}
Result<std::string, ParsingError> Serializer<std::string>::deserialize(const ByteArray& data)
{
    try {
        return Ok{json::parse(data.value()).as_string()};
    }
    catch (const std::exception& e) {
        return Fail{e.what()};
    }
}

ByteArray Serializer<Request>::serialize(const Request& req)
{
    json::array args;
    for (auto&& arg : req.arguments) args.push_back(json::string{arg.value()});

    // clang-format off
    json::object obj{
        {"request_id", req.request_id.value()},
        {"name_space", req.name_space},
        {"function_name", req.function_name},
        {"arguments", args}
    };
    // clang-format on

    return ByteArray{json::serialize(obj)};
}

Result<Request, ParsingError> Serializer<Request>::deserialize(const ByteArray& data)
{
    try {
        Request result;

        json::value val = json::parse(data.value());
        json::object& obj = val.as_object();

        result.request_id = RequestId{obj.at("request_id").as_string()};
        result.name_space = obj.at("name_space").as_string();
        result.function_name = obj.at("function_name").as_string();

        for (auto& arg : obj.at("arguments").as_array()) {
            result.arguments.push_back(ByteArray{arg.as_string()});
        }

        return Ok{std::move(result)};
    }
    catch (const std::exception& e) {
        return Fail{e.what()};
    }
}

///////////////////////////////////////////////////////////////////////////////

ByteArray Serializer<Reply>::serialize(const Reply& rep)
{
    // clang-format off
    json::object obj{
        {"request_id", rep.request_id.value()},
        {"result", rep.result.value()},
        {"type", std::to_underlying(rep.type)}
    };
    // clang-format on

    return ByteArray{json::serialize(obj)};
}

Result<Reply, ParsingError> Serializer<Reply>::deserialize(const ByteArray& data)
{
    try {
        Reply result;

        json::value val = json::parse(data.value());
        json::object& obj = val.as_object();

        result.request_id = RequestId{obj.at("request_id").as_string()};
        result.result = ByteArray{obj.at("result").as_string()};
        result.type = static_cast<ReplyType>(obj.at("type").as_int64());

        return Ok{std::move(result)};
    }
    catch (const std::exception& e) {
        return Fail{e.what()};
    }
}

///////////////////////////////////////////////////////////////////////////////

ByteArray Serializer<Error>::serialize(const Error& error)
{
    // clang-format off
    json::object obj{
        {"kind", error.kind()},
        {"message", error.what()}
    };
    // clang-format on

    return ByteArray{json::serialize(obj)};
}

Result<Error, ParsingError> Serializer<Error>::deserialize(const ByteArray& data)
{
    try {
        json::value val = json::parse(data.value());
        json::object& obj = val.as_object();
        Error::Kind kind = static_cast<Error::Kind>(obj.at("kind").as_int64());
        Error result{kind, std::string{obj.at("message").as_string()}};
        return Ok{std::move(result)};
    }
    catch (const std::exception& e) {
        return Fail{e.what()};
    }
}

}  // namespace ez::rpc
