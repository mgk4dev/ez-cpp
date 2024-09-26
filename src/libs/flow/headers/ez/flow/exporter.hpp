#pragma once

#include <ez/flow/parser.hpp>

#include <iosfwd>

namespace ez::flow {
struct JsonExporter {
    std::reference_wrapper<std::ostream> ostream;
    void run(const Program<ast::Program>&);
};

}  // namespace ez::flow
