#include "Function.hpp"

#include "../Entity.hpp"
#include "CallArguments.hpp"

namespace ez::flow::engine {
Constructor::Constructor() {}
Constructor::Constructor(Callable impl) : call{std::move(impl)} {}
EvalResult Constructor::operator()(Entity dict) const { return call(std::move(dict)); }

///////////////////////////////////////////////////////////////////////////////////////////////

Assignment::Assignment()
{
    call = [](Entity& lhs, const Entity& rhs) -> EvalResult {
        if (!same_type(lhs.type(), rhs.type()) && !same_type(rhs.type(), Error::static_type())) {
            return error::invalid_assignment(lhs.type().name, rhs.type().name);
        }
        lhs = rhs;
        return Ok{lhs};
    };
}
Assignment::Assignment(Callable impl) : call{std::move(impl)} {}
EvalResult Assignment::operator()(Entity& lhs, const Entity& rhs) const { return call(lhs, rhs); }

///////////////////////////////////////////////////////////////////////////////////////////////

Representation::Representation()
{
    call = [](const Entity& arg) -> std::string {
        return std::format("{}({})", arg.type().name, arg.address());
    };
}
Representation::Representation(Callable impl) : call{std::move(impl)} {}
std::string Representation::operator()(const Entity& arg) const { return call(arg); }

///////////////////////////////////////////////////////////////////////////////////////////////

Convert::Convert()
{
    call = [](const Entity& arg, const Type& type) -> EvalResult {
        if (!same_type(arg.type(), type)) {
            return error::type_not_convertible(arg.type().name, type.name);
        }
        return Ok{arg};
    };
}
Convert::Convert(Callable impl) : call{std::move(impl)} {}
EvalResult Convert::operator()(const Entity& arg, const Type& type) const
{
    return call(arg, type);
}

///////////////////////////////////////////////////////////////////////////////////////////////

UnaryOp::UnaryOp(std::string_view symbol)
    : UnaryOp{[symbol](const Entity& arg) { return error::op_not_found(symbol, arg.type().name); }}
{
}

UnaryOp::UnaryOp(Callable impl) : call{std::move(impl)} {}
EvalResult UnaryOp::operator()(const Entity& var) const { return call(var); }

/////////////////////////////////////////////////////////////////////////////////////////////

BinaryOp::BinaryOp(std::string_view symbol)
    : BinaryOp{[symbol](const Entity& lhs, const Entity& rhs) {
          return error::op_not_found(symbol, lhs.type().name, rhs.type().name);
      }}
{
}

BinaryOp::BinaryOp(Callable impl) : call{std::move(impl)} {}
EvalResult BinaryOp::operator()(const Entity& lhs, const Entity& rhs) const
{
    return call(lhs, rhs);
}

/////////////////////////////////////////////////////////////////////////////////////////////

EZ_FLOW_TYPE_IMPL(FreeFunction)
{
    Type type;
    type.name = "free function";
    return type;
}

EvalResult FreeFunction::call(CallArguments args) const { return call_impl(std::move(args)); }

EZ_FLOW_TYPE_IMPL(MemberFunction)
{
    Type type;
    type.name = "member function";
    return type;
}

EvalResult MemberFunction::call(Entity& self, CallArguments args) const
{
    return call_impl(self, std::move(args));
}

}  // namespace ez::flow::engine
