#include "CallArguments.hpp"

#include "../Entity.hpp"

#include <algorithm>
#include <ranges>

namespace ez::flow::engine {
void CallArguments::push(Entity&& value)
{
    push_back(CallArgument(size(), none, std::move(value)));
}

void CallArguments::push(const std::string& name, Entity&& value)
{
    push_back(CallArgument(size(), name, std::move(value)));
}

Entity* CallArguments::find(std::string_view name)
{
    for (auto&& arg : *this) {
        if (arg.name && arg.name.value() == name) return &arg.value;
    }
    return nullptr;
}

const Entity* CallArguments::find(std::string_view name) const
{
    for (auto&& arg : *this) {
        if (arg.name && arg.name.value() == name) return &arg.value;
    }
    return nullptr;
}

Entity& CallArguments::variable_at(size_t index) { return at(index).value; }

bool CallArguments::is_array() const
{
    return std::ranges::all_of(
        *this, [](auto&& name) { return !name.has_value(); }, &CallArgument::name);
}

bool CallArguments::is_dictionary() const
{
    return std::ranges::all_of(
        *this, [](auto&& name) { return name.has_value(); }, &CallArgument::name);
}

Dictionary CallArguments::to_dictionary() const
{
    Dictionary dict;

    for (auto&& arg : *this) {
        if (arg.name) dict.value().insert({arg.name.value(), arg.value});
    }

    return dict;
}

}  // namespace ez::flow::engine
