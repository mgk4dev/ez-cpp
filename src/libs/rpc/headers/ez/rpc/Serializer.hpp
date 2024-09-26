#pragma once

#include <ez/rpc/Types.hpp>

#include <ez/Result.hpp>
#include <ez/Traits.hpp>

#include <vector>

namespace ez::rpc {
template <typename T>
struct Serializer;

#define EZ_RPC_POD_SERIALIZER(T)                                      \
    template <>                                                       \
    struct Serializer<T> {                                            \
        static ByteArray serialize(T);                                \
        static Result<T, ParsingError> deserialize(const ByteArray&); \
    }

#define EZ_RPC_SERIALIZER(T)                                          \
    template <>                                                       \
    struct Serializer<T> {                                            \
        static ByteArray serialize(const T&);                         \
        static Result<T, ParsingError> deserialize(const ByteArray&); \
    }

///////////////////////////////////////////////////////////////////////////////

EZ_RPC_POD_SERIALIZER(std::uint16_t);
EZ_RPC_POD_SERIALIZER(std::uint32_t);
EZ_RPC_POD_SERIALIZER(std::uint64_t);

EZ_RPC_POD_SERIALIZER(std::int16_t);
EZ_RPC_POD_SERIALIZER(std::int32_t);
EZ_RPC_POD_SERIALIZER(std::int64_t);
EZ_RPC_POD_SERIALIZER(float);
EZ_RPC_POD_SERIALIZER(double);
EZ_RPC_POD_SERIALIZER(bool);

EZ_RPC_SERIALIZER(std::string);
// EZ_RPC_SERIALIZER(Error);

//////////////////////////////////////////////////////////////////////////////

template <typename T>
concept ProtobufLike = requires(T obj)
{
    {
        obj.SerializeAsString()
    }
    ->trait::Is<std::string>;
    {
        obj.ParseFromString(std::declval<std::string>())
    }
    ->trait::Is<bool>;
};

template <ProtobufLike T>
struct Serializer<T> {
    static ByteArray serialize(const T& val) { return ByteArray{val.SerializeAsString()}; }
    static Result<T, ParsingError> deserialize(const ByteArray& bytes)
    {
        T val;
        if (val.ParseFromString(bytes.value())) return Ok{std::move(val)};
        return Fail{"Failed to parse bytes"};
    }
};

//////////////////////////////////////////////////////////////////////////////

template <typename T>
ByteArray serialize(const T& val)
{
    return Serializer<T>::serialize(val);
}

template <typename T>
Result<T, ParsingError> deserialize(const ByteArray& data)
{
    return Serializer<T>::deserialize(data);
}

}  // namespace ez::rpc
