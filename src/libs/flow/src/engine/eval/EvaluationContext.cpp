#include "EvaluationContext.hpp"

#include "../types/builtin/Functions.hpp"

namespace ez::flow::engine {
void Scope::add(Type type) { types.push_back(std::move(type)); }

void Scope::add(FreeFunction function) { entries.push_back({function.name, function}); }

void Scope::add(std::string name, Entity value)
{
    entries.push_back({std::move(name), std::move(value)});
}

VariableEntry* Scope::find_entity(std::string_view name)
{
    for (auto it = entries.rbegin(); it != entries.rend(); ++it) {
        if (it->name == name) return &*it;
    }

    return nullptr;
}

Type* Scope::find_type(std::string_view name)
{
    for (auto it = types.rbegin(); it != types.rend(); ++it) {
        if (it->name == name) return &*it;
    }
    return nullptr;
}

///////////////////////////////////////////////////////////////////////////////

EvaluationScope::EvaluationScope()
{
    auto& scope = push(ScopeType::Function);

    meta::for_each(EntityTypeList{}, [&]<typename T>(ez::Type<T>) { scope.add(T::static_type()); });

    scope.add(make_println_function());
    scope.add(make_panic_function());
    scope.add(make_str_format_function());
}

void EvaluationScope::return_value(Entity value)
{
    current().return_value = std::move(value);
    current().flow_control_policy = FlowControlPolicy::Return;
}

void EvaluationScope::break_loop() { current().flow_control_policy = FlowControlPolicy::Break; }

Scope& EvaluationScope::push(ScopeType mode)
{
    switch (mode) {
        case ScopeType::Local:
            return m_scopes.emplace_back(current().return_value, ScopeType::Local);
        case ScopeType::Loop: return m_scopes.emplace_back(current().return_value, ScopeType::Loop);
        case ScopeType::Function:
            return m_scopes.emplace_back(Option<Entity>{}, ScopeType::Function);
    }
    std::terminate();
}

void EvaluationScope::pop()
{
    auto scope_type = current().type;
    auto flow_control_policy = current().flow_control_policy;
    m_scopes.pop_back();

    if (flow_control_policy == FlowControlPolicy::Stay) return;

    if (flow_control_policy == FlowControlPolicy::Return && scope_type == ScopeType::Loop) {
        current().flow_control_policy = FlowControlPolicy::Return;
    }
    else if (flow_control_policy == FlowControlPolicy::Return && scope_type == ScopeType::Local) {
        current().flow_control_policy = FlowControlPolicy::Return;
    }
    else if (flow_control_policy == FlowControlPolicy::Break && scope_type == ScopeType::Loop) {
        current().flow_control_policy = FlowControlPolicy::Break;
    }
    else if (flow_control_policy == FlowControlPolicy::Break && scope_type == ScopeType::Local) {
        current().flow_control_policy = FlowControlPolicy::Break;
    }
}

Scope& EvaluationScope::current() { return m_scopes.back(); }

std::size_t EvaluationScope::scope_count() const { return m_scopes.size(); }

VariableEntry* EvaluationScope::find_entity(std::string_view name)
{
    for (auto it = m_scopes.rbegin(); it != m_scopes.rend(); ++it) {
        if (auto entry = it->find_entity(name); entry) return entry;
    }

    return nullptr;
}

Type* EvaluationScope::find_type(std::string_view name)
{
    for (auto it = m_scopes.rbegin(); it != m_scopes.rend(); ++it) {
        if (auto entry = it->find_type(name); entry) return entry;
    }

    return nullptr;
}

}  // namespace ez::flow::engine
