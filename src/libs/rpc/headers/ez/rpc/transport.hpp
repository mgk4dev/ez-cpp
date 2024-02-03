#pragma once

#include <ez/rpc/types.hpp>

namespace ez::rpc::transport {

struct Client {
    virtual ~Client() = default;
    virtual Result<void, std::runtime_error> connect(const std::string&) = 0;
    virtual AsyncResult<> send(const ByteArray&) = 0;
    virtual bool receive(ByteArray&) = 0;
};

struct Server {
    struct Message {
        PeerId peer_id;
        ByteArray payload;
    };

    virtual ~Server() = default;
    virtual Result<void, std::runtime_error> bind_to(const std::string&) = 0;
    virtual AsyncResult<> send(const PeerId& peer_id, const ByteArray& payload) = 0;
    virtual bool receive(Message&) = 0;
};

}  // namespace ez::rpc::transport
