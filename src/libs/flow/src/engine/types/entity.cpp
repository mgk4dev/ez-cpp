#include "entity.hpp"

#include "core/call_arguments.hpp"

#include <ez/Preprocessor.hpp>

namespace ez::flow::engine {
#define EZ_FLOW_APPLY_TO(arg) (arg) >> [&](auto&& self) -> decltype(auto)

#define EZ_FLOW_APPLY EZ_FLOW_APPLY_TO(*this)

/////////////////////////////////////////////////////////////////////////////////

EntityCategory Entity::category() const
{
    return EZ_FLOW_APPLY { return self.category(); };
}

const Type& Entity::type() const
{
    return EZ_FLOW_APPLY { return self.type(); };
}

const void* Entity::address() const
{
    return EZ_FLOW_APPLY { return (const void*)std::addressof(self); };
}

Entity Entity::copy() const&
{
    return EZ_FLOW_APPLY
    {
        auto copy = self;
        copy.detach();
        return Entity{std::move(copy)};
    };
}

Entity Entity::copy() &&
{
    return EZ_FLOW_APPLY
    {
        if (!self.is_unique()) self.detach();
        return Entity{std::move(self)};
    };
}

}  // namespace ez::flow::engine
