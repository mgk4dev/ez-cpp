#pragma once

#include <ez/Option.hpp>

#include <cstdint>
#include <span>
#include <string>

namespace ez {

using ByteArrayView = std::span<const std::uint8_t>;

struct ByteArray : public std::basic_string<std::uint8_t> {
    using Super = std::basic_string<std::uint8_t>;

    using Super::Super;

    ByteArray(std::size_t count) { resize(count); }

    std::string_view as_string() const
    {
        return std::string_view{reinterpret_cast<const char*>(data()),
                                reinterpret_cast<const char*>(data()) + size()};
    }

    std::string to_hex_string() const
    {
        std::string result;
        result.resize(size() * 2);
        for (size_t i = 0; i < size(); i++)
            std::sprintf(&result[i * 2], "%02x", this->operator[](i));
        return result;
    }

    static Option<ByteArray> from_hex_string(std::string_view string)
    {
        if (string.size() % 2) return none;

        ByteArray result;
        result.resize(string.size() / 2);

        const auto char2int = [](char input) -> Option<int> {
            if (input >= '0' && input <= '9') return input - '0';
            if (input >= 'A' && input <= 'F') return input - 'A' + 10;
            if (input >= 'a' && input <= 'f') return input - 'a' + 10;
            return none;
        };

        for (size_t i = 0; i < result.size(); i++) {
            const auto left = char2int(string[i * 2]);
            const auto right = char2int(string[i * 2 + 1]);
            if (!left || !right) return none;
            result[i] = *left * 16 + *right;
        }
        return result;
    }

    template <typename T>
    T to_pod_bytes() const
    {
        T result = 0;
        std::span<std::uint8_t> span{reinterpret_cast<std::uint8_t*>(&result),
                                     reinterpret_cast<std::uint8_t*>(&result) + sizeof(T)};

        std::copy(begin(), end(), span.rbegin());

        return result;
    }

    template <typename T>
    static ByteArray from_pod_bytes(T val)
    {
        ByteArray result;
        result.resize(sizeof(T));

        std::span<const std::uint8_t> span{reinterpret_cast<const std::uint8_t*>(&val),
                                           reinterpret_cast<const std::uint8_t*>(&val) + sizeof(T)};

        std::copy(span.rbegin(), span.rend(), result.begin());
        return result;
    }
};

}  // namespace ez
