#pragma once

#include <ez/flow/Parser.hpp>

#include <ez/Utils.hpp>

#include <iosfwd>

namespace ez::flow {
struct JsonExporter {
    Ref<std::ostream> ostream;
    void run(const Program<ast::Program>&);
};

}  // namespace ez::flow
