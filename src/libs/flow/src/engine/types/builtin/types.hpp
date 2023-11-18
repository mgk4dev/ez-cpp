#pragma once

#include "../core/value_interface.hpp"

#include <ez/flow/statement.hpp>

#include <map>
#include <string>
#include <vector>

namespace ez::flow::engine {

struct Error : Value<> {
    EZ_FLOW_TYPE(EntityCategory::Error)
    std::string what;
    Statement<ast::RaiseStatement> where;
};

struct Void : Value<> {
    EZ_FLOW_TYPE(EntityCategory::Object)
    using ValueType::ValueType;
    using ValueType::operator=;
};

struct Boolean : Value<bool> {
    EZ_FLOW_TYPE(EntityCategory::Object)
    using ValueType::ValueType;
    using ValueType::operator=;
};

struct Integer : Value<std::int64_t> {
    EZ_FLOW_TYPE(EntityCategory::Object)
    using ValueType::ValueType;
    using ValueType::operator=;
};

struct Real : Value<double> {
    EZ_FLOW_TYPE(EntityCategory::Object)
    using ValueType::ValueType;
    using ValueType::operator=;
};

struct String : Value<std::string> {
    EZ_FLOW_TYPE(EntityCategory::Object)
    using ValueType::ValueType;
    using ValueType::operator=;
};

struct DurationStorage {
    std::uint64_t count = 0;
    ast::DurationUnit unit = ast::DurationUnit::Ms;
};

struct Duration : Value<DurationStorage> {
    EZ_FLOW_TYPE(EntityCategory::Object)
    using ValueType::ValueType;
    using ValueType::operator=;
};

struct Array : Value<std::vector<Entity>> {
    EZ_FLOW_TYPE(EntityCategory::Object)
    using ValueType::ValueType;
    using ValueType::operator=;
};

using DictionaryMap = std::map<std::string, Entity, std::less<>>;

struct Dictionary : Value<DictionaryMap> {
    EZ_FLOW_TYPE(EntityCategory::Object)
    using ValueType::ValueType;
    using ValueType::operator=;
};

}  // namespace ez::flow::engine
