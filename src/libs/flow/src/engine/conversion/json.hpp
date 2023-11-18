#pragma once

#include "../types/builtin/types.hpp"

#include <boost/json.hpp>

namespace ez::flow::engine {

using JsonObject = boost::json::object;
using JsonArray = boost::json::array;
using JsonValue = boost::json::value;

inline JsonObject to_json(const Dictionary& dict);

inline JsonArray to_json(const Array& array)
{
    JsonArray result;

    for (const auto& value : array.value()) {
        value.match([&](const Boolean& value) { result.push_back(value.value()); },
                    [&](const Integer& value) { result.push_back(value.value()); },
                    [&](const Real& value) { result.push_back(value.value()); },
                    [&](const String& value) { result.push_back(JsonValue{value.value()}); },
                    [&](const Array& value) { result.push_back(to_json(value.value())); },
                    [&](const Dictionary& value) { result.push_back(to_json(value.value())); },
                    [&](const auto&) {}
        );
    }

    return result;
}

inline JsonObject to_json(const Dictionary& dict)
{
    JsonObject result;

    for (const auto& [key, value] : dict.value()) {
        value.match([&](const Boolean& value) { result[key] = value.value(); },
                    [&](const Integer& value) { result[key] = value.value(); },
                    [&](const Real& value) { result[key] = value.value(); },
                    [&](const String& value) { result[key] = value.value(); },
                    [&](const Array& value) { result[key] = to_json(value.value()); },
                    [&](const Dictionary& value) { result[key] = to_json(value.value()); },
                    [&](const auto&) {}
        );
    }

    return result;
}

}  // namespace ez::flow::engine
