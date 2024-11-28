#include "Types.hpp"

#include "../builtin/Dictionary.hpp"

namespace ez::flow::engine {
EZ_FLOW_TYPE_IMPL(HttpRequest)
{
    return DictionaryType<HttpRequest, [] { return "http_request"; }>::construct_type();
}

EZ_FLOW_TYPE_IMPL(HttpReply)
{
    return DictionaryType<HttpReply, [] { return "http_reply"; }>::construct_type();
}

}  // namespace ez::flow::engine
