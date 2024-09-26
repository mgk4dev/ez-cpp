#pragma once

#include <ez/flow/program.hpp>

#include <string>

namespace ez::flow {
Program<ast::Program> parse(std::string_view code, std::string_view file_name = {});

}  // namespace ez::flow
