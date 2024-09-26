#pragma once

#include <ez/flow/ast.hpp>

namespace ez::flow {
template <typename Ast>
struct Program {
    std::string code;
    std::string file_path;
    Ast ast;
    std::string error_message;
    bool valid = true;

    operator bool() const { return valid; }

    std::string_view get_code(const ast::Located& location) const
    {
        return std::string_view{code}.substr(location.start_position,
                                             location.end_position - location.start_position);
    }

    template <typename T>
    std::string_view get_code(const Option<T>& statement) const
    {
        static const std::string_view unset = "unset";
        if (statement) { return get_code(statement.value()); }
        return unset;
    }
};

}  // namespace ez::flow
