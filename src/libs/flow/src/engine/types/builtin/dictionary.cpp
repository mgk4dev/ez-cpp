#include "types.hpp"

#include "../entity_utils.hpp"

namespace ez::flow::engine {

namespace dictionary {

std::string representation(const Entity& self)
{
    const auto& elements = self.as<Dictionary>().value();
    std::string result = "(";
    uint index = 0;
    for (const auto& [name, value] : elements) {
        result += std::format("{}={}", name, entity::representation(value));
        if (++index != elements.size()) result += ", ";
    }
    result += ")";

    return result;
}

auto insert(Dictionary& self, CallArguments args) -> EvalResult
{
    if (args.size() != 2) {
        return error::invalid_function_call("dictionary", "insert", "expecting 2 arguments");
    }

    if (!args.is_array() && !args.is_array()) {
        return error::invalid_function_call("dictionary", "insert",
                                            "arguments should all be named or unnamed");
    }

    Entity* key;
    Entity* value;

    if (args.is_array()) {
        key = &args.variable_at(0);
        value = &args.variable_at(1);
    }
    else {
        key = args.find("key");
        value = args.find("value");
    }

    if (!key) { return error::invalid_function_call("dictionary", "insert", "no key provided"); }
    if (!value) {
        return error::invalid_function_call("dictionary", "insert", "no value provided");
    }

    if (!same_type(key->type(), String::static_type())) {
        return error::invalid_function_call("dictionary", "insert", "expected a string key");
    }

    self->insert({std::move(key->as<String>().value()), std::move(*value)});

    return Ok{Void{}};
}

EvalResult dynamic_properties(Entity& self, std::string_view name)
{
    auto& elements = self.as<Dictionary>().value();

    if (auto iter = elements.find(name); iter != elements.end()) {
        Entity& var = iter->second;
        return Ok{var};
    }

    return error::property_not_found(name);
}

EvalResult contains(const Entity& self, const Entity& element)
{
    if (!element.is<String>()) {
        return error::generic("Expected a string, got a {}", element.type().name);
    }

    auto& elements = self.as<Dictionary>().value();

    return Ok{Boolean{elements.contains(element.as<String>().value())}};
}

}  // namespace dictionary

EZ_FLOW_TYPE_IMPL(Dictionary)
{
    Type result;
    result.id = result.name = "dictionary";
    result.construct.call = entity::make_constructor<Dictionary>();
    result.representation.call = dictionary::representation;
    result.binary_op_contains.call = dictionary::contains;
    result.static_properties = {EZ_FLOW_STATIC_PROPERTY(Dictionary, Integer, "integer", "size") =
                                    [](Dictionary& self) -> Integer { return self->size(); }};

    result.member_functions = {EZ_FLOW_MEM_FN(Dictionary, "dictionary", "insert") =
                                   dictionary::insert};

    result.dynamic_properties = dictionary::dynamic_properties;

    return result;
}

}  // namespace ez::flow::engine
