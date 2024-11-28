#include "Type.hpp"

#include "../Entity.hpp"
#include "CallArguments.hpp"

namespace ez::flow::engine {
EvalResult Type::get_property(Entity& self, std::string_view property_name) const
{
    if (auto iter = static_properties.find(property_name); iter != static_properties.end()) {
        return iter->second.get(self);
    }

    if (auto iter = member_functions.find(property_name); iter != member_functions.end()) {
        return Ok{iter->second};
    }

    if (dynamic_properties) { return dynamic_properties(self, property_name); }

    return error::property_not_found(property_name);
}

EvalResult Type::call_mem_function(Entity& self, std::string_view name, CallArguments args) const
{
    if (auto iter = member_functions.find(name); iter != member_functions.end()) {
        return iter->second.call(self, std::move(args));
    }

    return error::member_function_not_found(name);
}

}  // namespace ez::flow::engine
