#include <ez/rpc/Serializer.hpp>

#include "protobuf/messages.pb.h"

#include <google/protobuf/wrappers.pb.h>

namespace ez::rpc {
template <typename>
struct ProtobufWrapper;

#define EZ_RPC_MAP_TYPE(T, P)   \
    template <>                 \
    struct ProtobufWrapper<T> { \
        using Type = P;         \
    }

EZ_RPC_MAP_TYPE(bool, google::protobuf::BoolValue);
EZ_RPC_MAP_TYPE(double, google::protobuf::DoubleValue);
EZ_RPC_MAP_TYPE(float, google::protobuf::FloatValue);
EZ_RPC_MAP_TYPE(std::string, google::protobuf::StringValue);
EZ_RPC_MAP_TYPE(int32_t, google::protobuf::Int32Value);
EZ_RPC_MAP_TYPE(int64_t, google::protobuf::Int64Value);
EZ_RPC_MAP_TYPE(uint32_t, google::protobuf::UInt32Value);
EZ_RPC_MAP_TYPE(uint64_t, google::protobuf::UInt64Value);

#define EZ_RPC_PRIMITIVE_SERIALIZER(T)                                            \
    ByteArray Serializer<T>::serialize(const T& val)                              \
    {                                                                             \
        typename ProtobufWrapper<T>::Type proto;                                  \
        proto.set_value(val);                                                     \
        return ByteArray{proto.SerializeAsString()};                              \
    }                                                                             \
    Result<T, ParsingError> Serializer<T>::deserialize(const ByteArray& data)     \
    {                                                                             \
        typename ProtobufWrapper<T>::Type proto;                                  \
        if (proto.ParseFromString(data.value())) return std::move(proto.value()); \
        return Fail{"Failed to parse value"};                                     \
    }

#define EZ_RPC_PRIMITIVE_POD_SERIALIZER(T)                                        \
    ByteArray Serializer<T>::serialize(T val)                                     \
    {                                                                             \
        typename ProtobufWrapper<T>::Type proto;                                  \
        proto.set_value(val);                                                     \
        return ByteArray{proto.SerializeAsString()};                              \
    }                                                                             \
    Result<T, ParsingError> Serializer<T>::deserialize(const ByteArray& data)     \
    {                                                                             \
        typename ProtobufWrapper<T>::Type proto;                                  \
        if (proto.ParseFromString(data.value())) return std::move(proto.value()); \
        return Fail{"Failed to parse value"};                                     \
    }

EZ_RPC_PRIMITIVE_POD_SERIALIZER(bool)
EZ_RPC_PRIMITIVE_POD_SERIALIZER(double)
EZ_RPC_PRIMITIVE_POD_SERIALIZER(float)
EZ_RPC_PRIMITIVE_POD_SERIALIZER(int32_t)
EZ_RPC_PRIMITIVE_POD_SERIALIZER(int64_t)
EZ_RPC_PRIMITIVE_POD_SERIALIZER(uint32_t)
EZ_RPC_PRIMITIVE_POD_SERIALIZER(uint64_t)

EZ_RPC_PRIMITIVE_SERIALIZER(std::string)

// ByteArray Serializer<Error>::serialize(const Error& error)
// {
//     protobuf::Error proto;
//     proto.set_code(error.kind());
//     proto.set_what(error.what());
//     return serialize(proto);
// }

// Result<Error, ParsingError> Serializer<Error>::deserialize(const ByteArray& data)
// {
//     Result<protobuf::Error, ParsingError> proto = deserialize<protobuf::Error>(data);
//     if (!proto) return proto.error();
//     return Error{proto.value().code(), proto.value().what()};
// }

}  // namespace ez::rpc
