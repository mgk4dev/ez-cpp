#pragma once

#include <concepts>

#include <ez/rpc/messages.hpp>

namespace ez::rpc::trait {

template <typename T>
concept Serializer =  requires(T serializer) {
    { serializer.serialize(std::declval<RequestMessage>()) } -> Is<ByteArray>;
    { serializer.serialize(std::declval<ReplyMessage>()) } -> Is<ByteArray>;
};

}
