#include "evaluation_context.hpp"

#include "../types/builtin/functions.hpp"

namespace ez::flow::engine {

void Scope::add(Type type) { types.push_back(std::move(type)); }

void Scope::add(FreeFunction function) { entries.push_back({function.name, std::move(function)}); }

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
    auto& scope = push(PushScopeMode::New);

    for_each(EntityTypeList{}, [&]<typename T>(ez::Type<T>) { scope.add(T::static_type()); });

    scope.add(make_print_function());
}

Scope& EvaluationScope::push(PushScopeMode mode)
{
    Scope& scope = [&]() -> Scope& {
        switch (mode) {
            case PushScopeMode::Inherit: return m_scopes.emplace_back(current().return_value);
            case PushScopeMode::New: return m_scopes.emplace_back();
        }
    }();

    scope.mode = mode;
    return scope;
}

Scope& EvaluationScope::current() { return m_scopes.back(); }

void EvaluationScope::pop() { m_scopes.pop_back(); }

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
