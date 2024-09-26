#pragma once

#include <ez/Traits.hpp>
#include <ez/flow/ast.hpp>

#include <boost/spirit/home/x3.hpp>
#include <boost/spirit/home/x3/support/context.hpp>
#include <boost/spirit/home/x3/support/utility/error_reporting.hpp>

#include <ostream>

namespace ez::flow::parser {
namespace x3 = boost::spirit::x3;

struct HandlerTag;

struct SuccessHandler {
    template <typename T, typename Iterator, typename Context>
    inline void on_success(Iterator first, Iterator last, T& ast, const Context& context)
    {
        if constexpr (std::is_base_of_v<ast::Located, T>) {
            auto& handler = x3::get<HandlerTag>(context).get();
            auto code = handler.code;
            ast.start_position = std::distance(code.begin(), first);
            ast.end_position = std::distance(code.begin(), last);
            handler.on_statement_parsed(ast);
        }
        else if constexpr (trait::IsTemplate<T, Enum>) {
            ast.template match([&](auto& element) { on_success(first, last, element, context); });
        }
    }
};

template <typename Tag>
struct RuleTag : SuccessHandler {
};

}  // namespace ez::flow::parser
