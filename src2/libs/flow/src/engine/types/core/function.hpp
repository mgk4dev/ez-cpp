#pragma once

#include "value_interface.hpp"

#include <functional>

namespace ez::flow::engine {
struct Dictionary;

struct Constructor {
    using Callable = std::function<EvalResult(Entity)>;
    Callable call;

    Constructor();
    Constructor(Callable impl);
    EvalResult operator()(Entity args) const;
};

struct Assignment {
    using Callable = std::function<EvalResult(Entity&, const Entity&)>;
    Callable call;

    Assignment();
    Assignment(Callable impl);
    EvalResult operator()(Entity&, const Entity&) const;
};

struct Representation {
    using Callable = std::function<std::string(const Entity&)>;
    Callable call;

    Representation();
    Representation(Callable impl);
    std::string operator()(const Entity&) const;
};

struct Convert {
    using Callable = std::function<EvalResult(const Entity&, const Type&)>;
    Callable call;

    Convert();
    Convert(Callable impl);
    EvalResult operator()(const Entity&, const Type&) const;
};

/////////////////////////////////////////////////////////////////////////////

struct UnaryOp {
    using Callable = std::function<EvalResult(const Entity&)>;
    Callable call;

    UnaryOp(std::string_view symbol);
    UnaryOp(Callable impl);
    EvalResult operator()(const Entity& var) const;
};

/////////////////////////////////////////////////////////////////////////////

struct BinaryOp {
    using Callable = std::function<EvalResult(const Entity&, const Entity&)>;
    Callable call;

    BinaryOp(std::string_view symbol);
    BinaryOp(Callable impl);
    EvalResult operator()(const Entity& lhs, const Entity& rhs) const;
};

/////////////////////////////////////////////////////////////////////////////

struct FreeFunction : Value<> {
    EZ_FLOW_TYPE(EntityCategory::FreeFunction)
    using Callable = std::function<EvalResult(CallArguments)>;

    std::string name;
    Callable call_impl;

    EvalResult call(CallArguments) const;
};

/////////////////////////////////////////////////////////////////////////////

struct MemberFunction : Value<> {
    EZ_FLOW_TYPE(EntityCategory::MemberFunction)

    using Callable = std::function<EvalResult(Entity&, CallArguments)>;

    std::string type_name;
    std::string name;
    Callable call_impl;

    EvalResult call(Entity&, CallArguments) const;
};

/////////////////////////////////////////////////////////////////////////////

struct StaticProperty {
    using Callable = std::function<EvalResult(Entity&)>;
    std::string type_name;
    std::string name;
    Callable get;
};

using DynamicProperties = std::function<EvalResult(Entity&, std::string_view)>;

}  // namespace ez::flow::engine
