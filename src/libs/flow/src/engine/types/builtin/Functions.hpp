#pragma once

namespace ez::flow::engine {
struct FreeFunction;

FreeFunction make_println_function();
FreeFunction make_panic_function();
FreeFunction make_str_format_function();

}  // namespace ez::flow::engine
