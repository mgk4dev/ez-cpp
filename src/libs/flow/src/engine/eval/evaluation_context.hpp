#pragma once

#include "../types/core/function.hpp"
#include "../types/core/type.hpp"
#include "../types/entity.hpp"

#include <ez/reporting.hpp>

#include <deque>
#include <vector>

namespace ez::flow::engine {

struct VariableEntry {
    std::string name;
    Entity value;
};

enum class PushScopeMode { Inherit, New };

enum class FlowControlPolicy {
    NoExit,
    ExitCallScope,
    ExitCurrentScope,
};

struct Scope {
    Shared<Option<Entity>> return_value;
    FlowControlPolicy flow_control_policy = FlowControlPolicy::NoExit;
    std::deque<VariableEntry> entries;
    std::deque<Type> types;
    PushScopeMode mode = PushScopeMode::Inherit;

    void add(Type value);
    void add(FreeFunction value);
    void add(std::string name, Entity value);

    VariableEntry* find_entity(std::string_view name);
    Type* find_type(std::string_view name);
};

class EvaluationScope {
public:
    EvaluationScope();

    Scope& push(PushScopeMode);
    Scope& current();

    void pop();

    VariableEntry* find_entity(std::string_view name);
    Type* find_type(std::string_view name);

private:
    std::deque<Scope> m_scopes;
};

struct EvaluationContext {
    EvaluationScope scope;
    Logger logger;
};

}  // namespace ez::flow::engine
