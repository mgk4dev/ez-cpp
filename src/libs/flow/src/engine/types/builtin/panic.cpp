#include "functions.hpp"

#include <ez/flow/errors.hpp>

#include "../core/call_arguments.hpp"
#include "../entity_utils.hpp"

#include <sstream>

namespace ez::flow {
engine::FreeFunction engine::make_panic_function()
{
    FreeFunction panic;
    panic.name = "panic";

    panic.call_impl = [](CallArguments args) -> EvalResult {
        std::stringstream str;

        for (const CallArgument& arg : args) { str << entity::representation(arg.value) << " "; }

        throw Panic{str.str()};

        return Ok{Void{}};
    };

    return panic;
}

}  // namespace ez::flow
