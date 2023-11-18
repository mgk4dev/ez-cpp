#pragma once

#include "types.hpp"

#include <ez/enum.hpp>

namespace ez::flow::engine {

struct Reference;

struct Entity : public EntityTypeList::ApplyTo<Enum> {
    using EnumType::EnumType;
    EntityCategory category() const;
    const Type& type() const;
    const void* address() const;

    Entity copy() const&;
    Entity copy() &&;
};

struct CallArgument {
    uint index = 0;
    Option<std::string> name;
    Entity value;
};

}  // namespace ez::flow::engine
