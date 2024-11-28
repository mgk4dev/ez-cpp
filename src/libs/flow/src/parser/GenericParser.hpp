#pragma once

#include <ez/flow/Parser.hpp>

// #include "Debug.hpp"

#include "Annotations.hpp"
#include "grammar/Common.hpp"

#include <sstream>

namespace ez::flow {
namespace x3 = boost::spirit::x3;

template <typename Tag, typename T>
auto parse_using(const x3::rule<Tag, T>& input_rule,
                 std::string_view code,
                 std::string_view file_name = {}) -> Program<T>
{
    Program<T> result;
    result.code = std::string{code.begin(), code.end()};
    result.file_path = file_name;
    using Iter = std::string_view::iterator;

    Iter iter = code.begin();
    const Iter end = code.end();

    std::stringstream error_string_stream;

    using ErrorHandler = x3::error_handler<Iter>;
    ErrorHandler error_handler{iter, end, error_string_stream,
                               std::string{file_name.begin(), file_name.end()}};

    struct HandlerData {
        std::string_view code;
        std::size_t end_position = 0;
        void on_statement_parsed(const ast::Located& location)
        {
            end_position = std::max(end_position, location.end_position);
        }
    } handler{code};

    auto const annotated_rule = x3::with<parser::HandlerTag>(std::ref(handler))[input_rule];

    try {
        result.valid = x3::phrase_parse(iter, end, annotated_rule, grammar::skipper, result.ast);

        if (iter != end) {
            std::string message = "Parsing error !!\nExpecting: valid statement here:";
            error_handler(code.begin() + handler.end_position, message);
            result.valid = false;
        }
    }
    catch (const x3::expectation_failure<decltype(iter)>& exception) {
        std::string message = "Parsing error !!\nExpecting: " + exception.which() + " here:";

        error_handler(code.begin() + handler.end_position, message);
        result.valid = false;
    }

    result.error_message = error_string_stream.str();

    return result;
}
}  // namespace ez::flow
