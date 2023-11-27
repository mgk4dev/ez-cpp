#include <ez/flow/parser.hpp>

#include "grammar/grammar.hpp"

#include "generic_parser.hpp"

namespace ez::flow {
Program<ast::Program> parse(std::string_view code, std::string_view file_name)
{
    return parse_using(grammar::program, code, file_name);
}
}  // namespace ez::flow
