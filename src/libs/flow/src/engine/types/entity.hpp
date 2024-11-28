#pragma once

#include "types.hpp"

#include <cstdint>

namespace ez::flow::engine {
struct Reference;

struct Entity : public EntityTypeList::ApplyTo<OneOf> {
    using Super = EntityTypeList::ApplyTo<OneOf>;
    using Super::Super;

    EntityCategory category() const;
    const Type& type() const;
    const void* address() const;

    Entity copy() const&;
    Entity copy() &&;
};

struct CallArgument {
    std::uint32_t index = 0;
    Option<std::string> name;
    Entity value;
};

}  // namespace ez::flow::engine
