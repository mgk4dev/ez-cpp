#pragma once

#include "interpreter.hpp"

namespace ez::flow::engine {
Entity to_value(const ast::String& statement)
{
    String result;
    result = statement;
    return result;
}

Entity to_value(const ast::Integer& statement)
{
    Integer result;
    result = statement.value;
    return result;
}

Entity to_value(const ast::Real& statement)
{
    Real result;
    result = statement.value;
    return result;
}

Entity to_value(const ast::Duration& statement)
{
    Duration result;
    result->count = statement.count;
    result->unit = statement.unit;
    return result;
}

Entity to_value(bool value)
{
    Boolean result;
    result = value;
    return result;
}

}  // namespace ez::flow::engine
