#pragma once

#include "../EntityUtils.hpp"

namespace ez::flow::engine {
template <typename T, auto name>
struct DictionaryType {
    static EvalResult construct(Entity arg)
    {
        if (arg.is<Void>()) return T{};
        if (arg.is<T>()) return arg;
        if (arg.is<Error>()) return arg;
        if (arg.is<Dictionary>()) {
            T result;
            result.value() = arg.as<Dictionary>().value();
            return std::move(result);
        }

        return error::invalid_constructor(T::static_type().name, arg.type().name);
    };

    static std::string representation(const Entity& self)
    {
        const auto& elements = self.as<T>().value();
        std::string result = "(";
        unsigned int index = 0;
        for (const auto& [key, value] : elements) {
            result += std::format("{}={}", key, entity::representation(value));
            if (++index != elements.size()) result += ", ";
        }
        result += ")";

        return result;
    }

    static EvalResult insert(T& self, CallArguments args)
    {
        if (args.size() != 2) {
            return error::invalid_function_call(name(), "insert", "expecting 2 arguments");
        }

        if (!args.is_array() && !args.is_array()) {
            return error::invalid_function_call(name(), "insert",
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

        if (!key) { return error::invalid_function_call(name(), "insert", "no key provided"); }
        if (!value) { return error::invalid_function_call(name(), "insert", "no value provided"); }

        if (!same_type(key->type(), String::static_type())) {
            return error::invalid_function_call(name(), "insert", "expected a string key");
        }

        self->insert({std::move(key->as<String>().value()), std::move(*value)});

        return Void{};
    }

    static EvalResult dynamic_properties(Entity& self, std::string_view propety_name)
    {
        auto& elements = self.as<T>().value();

        if (auto iter = elements.find(propety_name); iter != elements.end()) {
            Entity& var = iter->second;
            return var;
        }

        return error::property_not_found(propety_name);
    }

    static EvalResult contains(const Entity& self, const Entity& element)
    {
        if (!element.is<String>()) {
            return error::generic("Expected a string, got a {}", element.type().name);
        }

        auto& elements = self.as<T>().value();

        return Boolean{elements.contains(element.as<String>().value())};
    }

    static Type construct_type()
    {
        Type result;
        result.id = result.name = name();
        result.construct.call = construct;
        result.representation.call = representation;
        result.binary_op_contains.call = contains;
        result.static_properties = {EZ_FLOW_STATIC_PROPERTY(T, Integer, "integer", "size") =
                                        [](T& self) -> Integer { return self->size(); }};

        result.member_functions = {EZ_FLOW_MEM_FN(T, name(), "insert") = insert};

        result.dynamic_properties = dynamic_properties;

        return result;
    }
};

}  // namespace ez::flow::engine
