#pragma once

#include <ez/flow/program.hpp>

namespace ez::flow::engine {

template <typename T>
struct Statement {
    const Program<ast::Program>* program_ptr = nullptr;
    const T* ast_ptr = nullptr;

    const Program<ast::Program>& program() const { return *program_ptr; }
    const T& ast() const { return *ast_ptr; }

    std::string_view get_code() const { return program().get_code(ast()); }

    template <typename U>
    Statement<U> make_statement(const U& ast) const
    {
        return {program_ptr, &ast};
    }

    template <typename U>
    Statement<U> operator>>(const U& ast) const
    {
        return {program_ptr, &ast};
    }

    template <typename U>
    Statement<U> as() const
    {
        return *this >> ast().template as<U>();
    }
};

template <typename T>
Statement<T> make_statement(const Program<ast::Program>& program, const T& ast)
{
    return {&program, &ast};
}

}  // namespace ez::flow::engine
