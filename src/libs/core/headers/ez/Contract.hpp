#pragma once

#include <ez/Preprocessor.hpp>
#include <ez/Os.hpp>

namespace ez::contract {
struct Expression {
    const char* value = nullptr;
    const char* file = nullptr;
    int line = 0;
};

struct Terminate {
    [[noreturn]] void operator()(const Expression& expression) const;
};
struct ThrowLogicError {
    [[noreturn]] void operator()(const Expression& expression) const;
};
struct PrintError {
    void operator()(const Expression& expression) const;
};

constexpr PrintError print_error{};
constexpr Terminate terminate{};
constexpr ThrowLogicError throw_logic_error{};

template <typename... Behaviors>
void check(bool condition, const Expression& expression, Behaviors&&... behaviors)
{
    if (condition) return;
    (..., behaviors(expression));
}

}  // namespace ez::contract

/// EZ_CONTRACT implements pre-condition checking. It is meant to replace C asserts as it allows to
/// customize the behavior when the precondition is not met. Examples: This code will just print an
/// error with the code location when the condition is not met
/// @code
/// EZ_CONTRACT( f() > 10, contract::printError);
/// @endcode
///
/// This code will throw a std::logic_error when the condition is not met
/// @code
/// EZ_CONTRACT( f() > 10, contract::throwLogicError);
/// @endcode
///
/// This code will terminate the program when the condition is not met
/// @code
/// EZ_CONTRACT( f() > 10, contract::terminate);
/// @endcode
///
///
/// It is possible to chain actions or use custom actions. The actions are chained from left to
/// right. Any action that terminate the program or throws will prevent remaining actions from
/// executing:
/// @code
/// double val = ...;
/// auto print = [](auto&&) {std::cerr << "val=" << val << std:endl;};
/// EZ_CONTRACT( val < 20, print, contract::terminate);
/// @endcode

// clang-format off
#define EZ_CONTRACT(condition, ...) ez::contract::check(bool(condition), ::ez::contract::Expression{EZ_STR(condition), __FILE__, __LINE__}, __VA_ARGS__)

#ifdef NDEBUG
#define EZ_ASSERT(condition)
#else
#define EZ_ASSERT(condition) EZ_CONTRACT(condition, ::ez::contract::print_error, ::ez::contract::terminate)
#endif

#if defined EZ_OS_WINDOWS
# define EZ_UNREACHABLE() __builtin_unreachable()
#else
# define EZ_UNREACHABLE() __assume(false)
#endif

// clang-format on
