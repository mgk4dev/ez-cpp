#include "value_interface.hpp"

#include "../entity.hpp"
#include "call_arguments.hpp"

namespace ez::flow::engine {
const Type& ValueInterface::type() const
{
    static const Type t;
    return t;
}

}  // namespace ez::flow::engine
