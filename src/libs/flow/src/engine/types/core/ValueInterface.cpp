#include "ValueInterface.hpp"

#include "../Entity.hpp"
#include "CallArguments.hpp"

namespace ez::flow::engine {
const Type& ValueInterface::type() const
{
    static const Type t;
    return t;
}

}  // namespace ez::flow::engine
