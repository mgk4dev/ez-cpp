#pragma once

#include "../types/builtin/types.hpp"

#include <boost/json.hpp>

namespace ez::flow::engine {

using JsonObject = boost::json::object;
using JsonArray = boost::json::array;
using JsonValue = boost::json::value;

inline JsonObject to_json(const Dictionary& dict);
inline Dictionary from_json(const JsonObject& dict);
inline JsonArray to_json(const Array& array)
{
    JsonArray result;

    for (const auto& value : array.value()) {
        value.match(
            [&](const Boolean& value) { result.push_back(value.value()); },
            [&](const Integer& value) { result.push_back(value.value()); },
            [&](const Real& value) { result.push_back(value.value()); },
            [&](const String& value) { result.push_back(boost::json::value_from(value.value())); },
            [&](const Array& value) { result.push_back(to_json(value.value())); },
            [&](const Dictionary& value) { result.push_back(to_json(value.value())); },
            [&](const auto&) {});
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
                    [&](const auto&) {});
    }

    return result;
}

inline Array from_json(const JsonArray& array)
{
    Array result;

    for (const auto& value : array) {
        if (value.is_bool())
            result.value().push_back(Boolean{value.as_bool()});
        else if (value.is_int64())
            result.value().push_back(Integer{value.as_int64()});
        else if (value.is_uint64())
            result.value().push_back(Integer{static_cast<std::int64_t>(value.as_uint64())});
        else if (value.is_double())
            result.value().push_back(Real{value.as_double()});
        else if (value.is_string())
            result.value().push_back(String{boost::json::value_to<std::string>(value)});
        else if (value.is_array())
            result.value().push_back(from_json(value.as_array()));
        else if (value.is_object())
            result.value().push_back(from_json(value.as_object()));
    }

    return result;
}

inline Dictionary from_json(const JsonObject& object)
{
    Dictionary result;

    for (const auto& [key, value] : object) {
        if (value.is_bool())
            result.value().insert({key, Boolean{value.as_bool()}});
        else if (value.is_int64())
            result.value().insert({key, Integer{value.as_int64()}});
        else if (value.is_uint64())
            result.value().insert({key, Integer{static_cast<std::int64_t>(value.as_uint64())}});
        else if (value.is_double())
            result.value().insert({key, Real{value.as_double()}});
        else if (value.is_string())
            result.value().insert({key, String{boost::json::value_to<std::string>(value)}});
        else if (value.is_array())
            result.value().insert({key, from_json(value.as_array())});
        else if (value.is_object())
            result.value().insert({key, from_json(value.as_object())});
    }

    return result;
}

}  // namespace ez::flow::engine
