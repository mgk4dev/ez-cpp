#include "types.hpp"

#include "dictionary_type.hpp"

namespace ez::flow::engine {
EZ_FLOW_TYPE_IMPL(Dictionary)
{
    return DictionaryType<Dictionary, [] { return "dictionary"; }>::construct_type();
}

}  // namespace ez::flow::engine
