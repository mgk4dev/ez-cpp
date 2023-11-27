#include "functions.hpp"

#include "../core/call_arguments.hpp"
#include "../entity_utils.hpp"

#include <boost/algorithm/string/replace.hpp>
#include <boost/format.hpp>

#include <iostream>

namespace ez::flow::engine {

Result<std::string, std::string> format_args(const CallArguments& args)
{
    if (args.size() < 1 || !args.front().value.is<String>()) {
        return Fail{"Invalid arguments !! Usage: ('base string', args ...) "};
    }

    if (args.size() == 1) { return Ok{args.front().value.as<String>().value()}; };

    std::string base_string = args.front().value.as<String>().value();
    try {
        for (std::size_t i = args.size() - 1; i > 0; --i) {
            boost::replace_all(base_string, "{" + std::to_string(i) + "}", std::format("%{}%", i));
        }

        boost::format fmt{base_string};

        for (std::size_t i = 1; i < args.size(); ++i) { fmt % entity::representation(args[i].value); }

        return Ok{fmt.str()};
    }
    catch (const std::exception&) {
        return Fail{"Invalid arguments !! Usage: ('base string', args ...) "};
    }
}

FreeFunction make_println_function()
{
    FreeFunction println;
    println.name = "println";

    println.call_impl = [](CallArguments args) -> EvalResult {
        const auto format = format_args(args);

        std::cout << "[FLOW] ";

        if (format) { std::cout << format.value(); }
        else {
            std::cout << format.error();
        }

        std::cout << std::endl << std::flush;

        return Ok{Void{}};
    };

    return println;
}

FreeFunction make_str_format_function()
{
    FreeFunction println;
    println.name = "str_format";

    println.call_impl = [](CallArguments args) -> EvalResult {
        const auto format = format_args(args);

        if (format) { return Ok{String{format.value()}}; }
        else {
            return Fail{format.error()};
        }
    };

    return println;
}

}  // namespace ez::flow::engine
