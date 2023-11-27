#include "types.hpp"

#include "../builtin/dictionary_type.hpp"

namespace ez::flow::engine {

EZ_FLOW_TYPE_IMPL(Action)
{
    return DictionaryType<Action, [] { return "action"; }>::construct_type();
}

EZ_FLOW_TYPE_IMPL(ActionReply)
{
    return DictionaryType<ActionReply, [] { return "action_reply"; }>::construct_type();
}

}  // namespace ez::flow::engine
