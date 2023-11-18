#pragma once

#include <span>
#include <string>
#include <vector>

namespace ez::flow::engine {

struct Entity;
struct Dictionary;
struct CallArgument;

struct CallArguments : public std::vector<CallArgument> {
    using std::vector<CallArgument>::vector;

    void push(Entity&&);
    void push(const std::string& name, Entity&&);

    Entity* find(std::string_view name);
    const Entity* find(std::string_view name) const;

    Entity& variable_at(size_t);

    bool is_array() const;
    bool is_dictionary() const;
    Dictionary to_dictionary() const;
};

struct CallableRequirements {
    uint arg_count;
};

}  // namespace ez::flow::engine
