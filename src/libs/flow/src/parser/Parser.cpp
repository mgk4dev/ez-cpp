#include <ez/flow/Parser.hpp>

#include "grammar/Grammar.hpp"

#include "GenericParser.hpp"

namespace ez::flow {
Program<ast::Program> parse(std::string_view code, std::string_view file_name)
{
    return parse_using(grammar::program, code, file_name);
}
}  // namespace ez::flow
