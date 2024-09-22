#pragma once

#include "errors.hpp"

#include <ez/Option.hpp>
#include <ez/Result.hpp>
#include <ez/Shared.hpp>

namespace ez::flow::engine {

struct Entity;
struct Type;
struct CallArguments;

using EvalResult = Result<Entity, EvaluationError>;

enum class EntityCategory {
    Error,
    Object,
    FreeFunction,
    MemberFunction,
    WorkflowDefinition,
    WorkflowInvocation,
    Type,
    Module
};

struct ValueInterface {
    virtual EntityCategory category() const = 0;
    virtual const Type& type() const;
};

template <typename Storage = Unit>
struct Value : public Shared<Storage>, public ValueInterface {
    using ValueType = Value;

    using Shared<Storage>::Shared;
    using Shared<Storage>::operator=;
};

#define EZ_FLOW_TYPE(category_)              \
    EntityCategory category() const override \
    {                                        \
        return category_;                    \
    }                                        \
    EZ_FLOW_TYPE_DECL

#define EZ_FLOW_TYPE_DECL             \
    static Type construct_type();     \
    static const Type& static_type(); \
    const Type& type() const override;

#define EZ_FLOW_TYPE_IMPL(T)                    \
    const Type& T::type() const                 \
    {                                           \
        return T::static_type();                \
    }                                           \
    const Type& T::static_type()                \
    {                                           \
        static const auto t = construct_type(); \
        return t;                               \
    }                                           \
    Type T::construct_type()

}  // namespace ez::flow::engine
