#include "functions.hpp"

#include "../core/call_arguments.hpp"
#include "../entity_utils.hpp"

#include <iostream>

namespace ez::flow {

engine::FreeFunction engine::make_print_function()
{
    FreeFunction print;
    print.name = "print";

    print.call_impl = [](CallArguments args) -> EvalResult {
        std::cout << "[FLOW] ";

        for (const CallArgument& arg : args) {
            std::cout << entity::representation(arg.value) << " ";
        }

        std::cout << std::endl;

        return Ok{Void{}};
    };

    return print;
}

}  // namespace ez::flow
