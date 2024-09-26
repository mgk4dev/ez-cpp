#pragma once

#include "core/call_arguments.hpp"
#include "entity.hpp"

namespace ez::flow::engine::entity {
inline std::string representation(const Entity& var) { return var.type().representation(var); }

inline EvalResult construct(const Type& type, Entity&& arguments)
{
    return type.construct(std::move(arguments));
}

inline EvalResult assign(Entity& lhs, const Entity& rhs) { return lhs.type().assign(lhs, rhs); }

inline EvalResult call_mem_function(Entity& lhs, std::string_view name, CallArguments&& args)
{
    return lhs.type().call_mem_function(lhs, name, std::move(args));
}

inline EvalResult convert(Entity& lhs, const Type& type) { return lhs.type().convert(lhs, type); }

inline EvalResult get_property(Entity& lhs, std::string_view name)
{
    return lhs.type().get_property(lhs, name);
}

inline const Entity& ensure_value(const EvalResult& result, ast::Located location)
{
    if (!result) throw result.error() << location;
    return result.value();
}

inline Entity&& ensure_value(EvalResult&& result, ast::Located location)
{
    if (!result) throw result.error() << location;
    return std::move(result).value();
}

template <typename T>
auto make_constructor(auto&&... emplace_args)
{
    return [default_value = T{EZ_FWD(emplace_args)...}](Entity args) -> EvalResult {
        if (args.is<Void>()) return Ok{default_value};
        if (args.is<T>()) return Ok{std::move(args)};
        return error::invalid_constructor(T::static_type().name, args.type().name);
    };
}

template <typename T>
struct MemberFunctionBuilder {
    using ExpectedSignature = EvalResult(T&, CallArguments);

    MemberFunction result;
    MemberFunctionBuilder(const std::string& type_name, const std::string& name)
    {
        result.type_name = type_name;
        result.name = name;
    }

    std::pair<std::string, MemberFunction> operator=(trait::Fn<ExpectedSignature> auto&& impl)
    {
        result.call_impl = [impl_ = EZ_FWD(impl)](Entity& self, CallArguments args) -> EvalResult {
            return impl_(self.as<T>(), std::move(args));
        };
        return {result.name, std::move(result)};
    }
};

template <typename T, typename R>
struct StaticPropertyBuilder {
    using ExpectedSignature = R(T&);

    StaticProperty result;
    StaticPropertyBuilder(const std::string& type_name, const std::string& name)
    {
        result.type_name = type_name;
        result.name = name;
    }

    std::pair<std::string, StaticProperty> operator=(trait::Fn<ExpectedSignature> auto&& impl)
    {
        result.get = [impl = EZ_FWD(impl)](Entity& self) -> EvalResult {
            return Ok{impl(self.as<T>())};
        };

        return {result.name, std::move(result)};
    }
};

#define EZ_FLOW_MEM_FN(T, type_name, name) \
    ez::flow::engine::entity::MemberFunctionBuilder<T> { type_name, name }

#define EZ_FLOW_STATIC_PROPERTY(T, R, type_name, name) \
    ez::flow::engine::entity::StaticPropertyBuilder<T, R> { type_name, name }

#define EZ_FLOW_BINARY_OP(T, op)                                                            \
    [](const Entity& lhs, const Entity& rhs) {                                              \
        return rhs.apply_visitor([&lhs](const auto& val) { return op(lhs.as<T>(), val); }); \
    }

#define EZ_FLOW_UNARY_OP(T, op) \
    [](const Entity& arg) { return arg.apply_visitor([](const auto& val) { return op(val); }); }

#define EZ_FLOW_UNARY_OP_NOT_SUPPORTED(op, symbol) \
    EvalResult op(const auto& arg) { return error::op_not_found(symbol, arg.type().name); }

#define EZ_FLOW_BINARY_OP_NOT_SUPPORTED(T, op, symbol)                        \
    EvalResult op(const T& lhs, const auto& rhs)                              \
    {                                                                         \
        return error::op_not_found(symbol, lhs.type().name, rhs.type().name); \
    }

#define EZ_FLOW_BINARY_OP_IMPL(name, R, Lhs, Rhs, op) \
    EvalResult name(const Lhs& lhs, const Rhs& rhs) { return Ok{R{lhs.value() op rhs.value()}}; }

}  // namespace ez::flow::engine::entity
