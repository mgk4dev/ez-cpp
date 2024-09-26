#pragma once

#include "../types/core/function.hpp"
#include "../types/core/type.hpp"
#include "../types/entity.hpp"

#include <ez/Logger.hpp>

#include <deque>
#include <vector>

namespace ez::flow::engine {
struct VariableEntry {
    std::string name;
    Entity value;
};

enum class ScopeType { Function, Loop, Local };

enum class FlowControlPolicy {
    Stay,
    Return,
    Break,
};

struct Scope {
    Shared<Option<Entity>> return_value;
    ScopeType type = ScopeType::Local;
    FlowControlPolicy flow_control_policy = FlowControlPolicy::Stay;
    std::deque<VariableEntry> entries;
    std::deque<Type> types;

    Scope(Shared<Option<Entity>> rv, ScopeType t) : return_value{std::move(rv)}, type{t} {}

    void add(Type value);
    void add(FreeFunction value);
    void add(std::string name, Entity value);

    VariableEntry* find_entity(std::string_view name);
    Type* find_type(std::string_view name);
};

class EvaluationScope {
public:
    EvaluationScope();

    void return_value(Entity);
    void break_loop();

    Scope& push(ScopeType);
    Scope& current();
    std::size_t scope_count() const;

    void pop();

    VariableEntry* find_entity(std::string_view name);
    Type* find_type(std::string_view name);

private:
    std::deque<Scope> m_scopes;
};

}  // namespace ez::flow::engine
