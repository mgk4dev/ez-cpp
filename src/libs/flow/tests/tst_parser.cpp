#include <gtest/gtest.h>

// #include "parser/debug.hpp"
#include "parser/generic_parser.hpp"
#include "parser/grammar/grammar.hpp"

#include <ez/meta.hpp>
#include <ez/strong_type.hpp>

using namespace ez;
using namespace ez::flow;

TEST(Parser, simple_quote_string)
{
    auto code = R"(
'hello'
)";
    auto result = flow::parse_using(grammar::single_quote_string, code);

    ASSERT_TRUE(result) << result.error_message;

    ASSERT_EQ(result.ast, "hello");
}

TEST(Parser, triple_quote_string)
{
    auto code = R"(
'''hello'''
)";
    auto result = flow::parse_using(grammar::triple_quote_string, code);

    ASSERT_TRUE(result) << result.error_message;

    ASSERT_EQ(result.ast, "hello");
}

TEST(Parser, string)
{
    {
        auto code = R"(
'hello'
)";
        auto result = flow::parse_using(grammar::string, code);

        ASSERT_TRUE(result) << result.error_message;

        ASSERT_TRUE(result.ast.is<ast::SingleQuoteString>());
        ASSERT_EQ(result.ast.as<ast::SingleQuoteString>(), "hello");
    }

    {
        auto code = R"(
'''hello'''
)";
        auto result = flow::parse_using(grammar::string, code);

        ASSERT_TRUE(result) << result.error_message;

        ASSERT_TRUE(result.ast.is<ast::TripleQuoteString>());
        ASSERT_EQ(result.ast.as<ast::TripleQuoteString>(), "hello");
    }
}

TEST(Parser, identifier)
{
    auto code = R"(
toto_478
)";
    auto result = flow::parse_using(grammar::identifier, code);

    ASSERT_TRUE(result) << result.error_message;

    ASSERT_EQ(result.ast, "toto_478");
}

TEST(Parser, identifier_path)
{
    auto code = R"(
toto_478.dd . de8
)";
    auto result = flow::parse_using(grammar::identifier_path, code);

    ASSERT_TRUE(result) << result.error_message;

    ASSERT_EQ(result.ast.size(), 3);
    ASSERT_EQ(result.ast[0], "toto_478");
    ASSERT_EQ(result.ast[1], "dd");
    ASSERT_EQ(result.ast[2], "de8");
}

TEST(Parser, integer)
{
    auto code = R"(
2598
)";
    auto result = flow::parse_using(grammar::integer, code);

    ASSERT_TRUE(result) << result.error_message;

    ASSERT_EQ(result.ast, 2598);
}

TEST(Parser, real)
{
    auto code = R"(
2598.548
)";
    auto result = flow::parse_using(grammar::real, code);

    ASSERT_TRUE(result) << result.error_message;

    ASSERT_EQ(result.ast, 2598.548);
}

TEST(Parser, duration)
{
    auto test_duration = [](auto code, uint count, ast::DurationUnit unit) {
        auto result = flow::parse_using(grammar::duration, code);

        ASSERT_TRUE(result) << result.error_message;

        ASSERT_EQ(result.ast.count, count);
        ASSERT_EQ(result.ast.unit, unit);
    };

    test_duration("22ms", 22, ast::DurationUnit::Ms);
    test_duration("22sec", 22, ast::DurationUnit::Sec);
    test_duration("22day", 22, ast::DurationUnit::Day);
    test_duration("22week", 22, ast::DurationUnit::Week);
    test_duration("22month", 22, ast::DurationUnit::Month);

    auto result = flow::parse_using(grammar::duration, "11 sec");

    ASSERT_FALSE(result);
}

TEST(Parser, literal)
{
    auto test_literal = []<typename ExpectedType>(auto code, Type<ExpectedType>) {
        auto result = flow::parse_using(grammar::literal, code);

        ASSERT_TRUE(result) << result.error_message;

        ASSERT_TRUE(result.ast.template is<ExpectedType>());
    };

    test_literal("'hello'", type<ast::SingleQuoteString>);
    test_literal("'''hello'''", type<ast::TripleQuoteString>);
    test_literal("2598", type<ast::Integer>);
    test_literal("2598.548", type<ast::Real>);

    test_literal("22ms", type<ast::Duration>);
    test_literal("22sec", type<ast::Duration>);
    test_literal("22day", type<ast::Duration>);
    test_literal("22week", type<ast::Duration>);
    test_literal("22month", type<ast::Duration>);
}

///////////////////////////////////////////////////////////////////////////////

TEST(Parser, expression)
{
    auto test_expression = [](auto code) {
        auto result = flow::parse_using(grammar::expression, code);
        ASSERT_TRUE(result) << code << ":" << result.error_message;
    };

    test_expression("85");
    test_expression("85.585");
    test_expression("85sec");
    test_expression("'hello'");
    test_expression("'''hello'''");
    test_expression("toto");
    test_expression("toto.titi");

    test_expression("+25");
    test_expression("-toto");
    test_expression("not false");

    test_expression("a + b");
    test_expression("a - b");
    test_expression("a * b");
    test_expression("a / b");
    test_expression("a + (b + c )");
    test_expression("a + (b / c )");

    test_expression("toto(a = 1, b = 'hello')");
    test_expression("(c = 1, d = 'hello')");
}

///////////////////////////////////////////////////////////////////////////////

TEST(Parser, declaration)
{
    auto code = "foo : bar";

    auto result = flow::parse_using(grammar::declaration, code);

    ASSERT_TRUE(result) << result.error_message;

    ASSERT_EQ(result.ast.name, "foo");
    ASSERT_EQ(result.ast.type, "bar");
}

TEST(Parser, signature)
{
    auto code = "(i1 : I1, i2 : I2) -> O";

    auto result = flow::parse_using(grammar::signature, code);

    ASSERT_TRUE(result) << result.error_message;

    const ast::Signature& signature = result.ast;

    ASSERT_EQ(signature.inputs.size(), 2);
    ASSERT_TRUE(signature.return_type);

    ASSERT_EQ(signature.inputs[0].name, "i1");
    ASSERT_EQ(signature.inputs[0].type, "I1");

    ASSERT_EQ(signature.inputs[1].name, "i2");
    ASSERT_EQ(signature.inputs[1].type, "I2");

    ASSERT_EQ(signature.return_type.value(), "O");
}

TEST(Parser, empty_signature)
{
    auto test_signature = [](auto code, uint input_cout, bool has_return_type) {
        auto result = flow::parse_using(grammar::signature, code);

        ASSERT_TRUE(result) << result.error_message;

        const ast::Signature& signature = result.ast;

        ASSERT_EQ(signature.inputs.size(), input_cout);
        ASSERT_EQ(bool(signature.return_type), has_return_type);
    };

    test_signature("()", 0, false);

    test_signature("(i1 : I1) ", 1, false);
    test_signature("(i1 : I1, i2 : I2) ", 2, false);

    test_signature("() -> O", 0, true);
}

TEST(Parser, dictionary)
{
    auto code = "(name = 'toto', age = 58day)";

    auto result = flow::parse_using(grammar::dictionary, code);

    ASSERT_TRUE(result) << result.error_message;

    const ast::Dictionary& dictionary = result.ast;

    ASSERT_EQ(dictionary.size(), 2);
    ASSERT_EQ(dictionary[0].name, "name");
    ASSERT_EQ(dictionary[1].name, "age");
}

TEST(Parser, dictionary_emtpy)
{
    auto code = "( )";

    auto result = flow::parse_using(grammar::dictionary, code);

    ASSERT_TRUE(result) << result.error_message;

    const ast::Dictionary& dictionary = result.ast;

    ASSERT_EQ(dictionary.size(), 0);
}

TEST(Parser, array)
{
    auto code = "['toto', 58day]";

    auto result = flow::parse_using(grammar::array, code);

    ASSERT_TRUE(result) << result.error_message;

    const ast::Array& array = result.ast;

    ASSERT_EQ(array.size(), 2);
}

TEST(Parser, array_emtpy)
{
    auto code = " [ ]";

    auto result = flow::parse_using(grammar::array, code);

    ASSERT_TRUE(result) << result.error_message;

    const ast::Array& array = result.ast;

    ASSERT_EQ(array.size(), 0);
}

/////////////////////////////////////////////////////

TEST(Parser, function_call)
{
    {
        auto code = " f(1, b = 2)  ";
        auto result = flow::parse_using(grammar::function_call, code);
        ASSERT_TRUE(result) << code;
        const ast::FunctionCall& function_call = result.ast;
        ASSERT_EQ(function_call.path.size(), 1);
        ASSERT_EQ(function_call.path[0], "f");

        ASSERT_EQ(function_call.arguments.size(), 2);
        ASSERT_FALSE(function_call.arguments[0].name);
        ASSERT_EQ(function_call.arguments[1].name, "b");
    }

    {
        auto code = " str_format('''test''', d_info_reply) ";
        auto result = flow::parse_using(grammar::function_call, code);
        ASSERT_TRUE(result) << code << " " << result.error_message;
        const ast::FunctionCall& function_call = result.ast;
        ASSERT_EQ(function_call.path.size(), 1);
        ASSERT_EQ(function_call.path[0], "str_format");

        ASSERT_EQ(function_call.arguments.size(), 2);
    }

    {
        auto code = " f()  ";
        auto result = flow::parse_using(grammar::function_call, code);
        ASSERT_TRUE(result) << code << " " << result.error_message;
        const ast::FunctionCall& function_call = result.ast;
        ASSERT_EQ(function_call.path.size(), 1);
        ASSERT_EQ(function_call.path[0], "f");

        ASSERT_EQ(function_call.arguments.size(), 0);
    }
}

///////////////////////////////////////////////////////////////////////////////

TEST(Parser, variable_declaration)
{
    {
        auto code = "var : action = (name = 'toto', age = 58day)";

        auto result = flow::parse_using(grammar::variable_declaration, code);

        ASSERT_TRUE(result) << result.error_message;

        const ast::VariableDeclaration& obj = result.ast;

        ASSERT_EQ(obj.name, "var");
        ASSERT_EQ(obj.type, "action");
    }
    {
        auto code = R"(
            log_failure : api_call = (
                payload = str_format('''test''', d_info_reply)
            )
)";

        auto result = flow::parse_using(grammar::variable_declaration, code);

        ASSERT_TRUE(result) << result.error_message;
    }

    {
        auto code = "uninstall_app : action = ( timeout = 5mn, id = '<id>' )";

        auto result = flow::parse_using(grammar::variable_declaration, code);

        ASSERT_TRUE(result) << result.error_message;

        const ast::VariableDeclaration& obj = result.ast;

        ASSERT_EQ(obj.name, "uninstall_app");
        ASSERT_EQ(obj.type, "action");
    }
}

TEST(Parser, variable_declaration_no_args)
{
    auto code = "var : action";

    auto result = flow::parse_using(grammar::variable_declaration, code);

    ASSERT_TRUE(result) << result.error_message;

    const ast::VariableDeclaration& obj = result.ast;

    ASSERT_EQ(obj.name, "var");
    ASSERT_EQ(obj.type, "action");
}

TEST(Parser, assignment)
{
    {
        auto code = "var = (name = 'toto', age = 58day)";

        auto result = flow::parse_using(grammar::assignment_statement, code);

        ASSERT_TRUE(result) << result.error_message;

        const ast::AssignmentStatement& obj = result.ast;

        ASSERT_EQ(obj.path.size(), 1);
        ASSERT_EQ(obj.path[0], "var");
    }

    {
        auto code = "var = str_format('''test''', d_info_reply)";

        auto result = flow::parse_using(grammar::assignment_statement, code);

        ASSERT_TRUE(result) << result.error_message;

        const ast::AssignmentStatement& obj = result.ast;

        ASSERT_EQ(obj.path.size(), 1);
        ASSERT_EQ(obj.path[0], "var");
    }
}

///////////////////////////////////////////////////////////////////////////////

TEST(Parser, workflow)
{
    auto code = R"(
myw : workflow (i1 : I1, i2 : I2) -> O = {
    ra : remote_action = (  a = 1, b = 2 ) ;
}
)";

    auto result = flow::parse_using(grammar::workflow_definition, code);

    ASSERT_TRUE(result) << result.error_message;

    const ast::WorkflowDefinition& workflow = result.ast;

    ASSERT_EQ(workflow.name, "myw");

    ASSERT_TRUE(workflow.signature);

    const ast::Signature& signature = workflow.signature.value();

    ASSERT_EQ(signature.inputs.size(), 2);

    ASSERT_EQ(signature.inputs[0].name, "i1");
    ASSERT_EQ(signature.inputs[0].type, "I1");

    ASSERT_EQ(signature.inputs[1].name, "i2");
    ASSERT_EQ(signature.inputs[1].type, "I2");

    ASSERT_EQ(signature.return_type.value(), "O");
}

TEST(Parser, workflow_no_signature)
{
    auto code = "myw : workflow = {}";

    auto result = flow::parse_using(grammar::workflow_definition, code);

    ASSERT_TRUE(result) << result.error_message;

    const ast::WorkflowDefinition& workflow = result.ast;

    ASSERT_EQ(workflow.name, "myw");
    ASSERT_FALSE(workflow.signature);
}

TEST(Parser, workflow_empty_signature)
{
    auto code = R"(myw : workflow () = {})";

    auto result = flow::parse_using(grammar::workflow_definition, code);

    ASSERT_TRUE(result) << result.error_message;

    const ast::WorkflowDefinition& workflow = result.ast;

    ASSERT_EQ(workflow.name, "myw");

    ASSERT_TRUE(workflow.signature);

    const ast::Signature& signature = workflow.signature.value();

    ASSERT_EQ(signature.inputs.size(), 0);
}

///////////////////////////////////////////////////////////////////////////////

TEST(Parser, import)
{
    auto code = "import root.leaf";
    auto result = flow::parse_using(grammar::import_statement, code);

    ASSERT_TRUE(result) << result.error_message;

    ASSERT_EQ(result.ast.path.size(), 2);
    ASSERT_EQ(result.ast.path[0], "root");
    ASSERT_EQ(result.ast.path[1], "leaf");
}

TEST(Parser, delay)
{
    auto code = "delay 5sec";
    auto result = flow::parse_using(grammar::delay_statement, code);

    ASSERT_TRUE(result) << result.error_message;
}

TEST(Parser, return)
{
    auto code = "return 5sec";
    auto result = flow::parse_using(grammar::return_statement, code);

    ASSERT_TRUE(result) << result.error_message;
}

TEST(Parser, raise)
{
    auto code = "raise 'error'";
    auto result = flow::parse_using(grammar::raise_statement, code);
    ASSERT_TRUE(result) << result.error_message;
}

TEST(Parser, try)
{
    {
        auto code = "await act";
        auto result = flow::parse_using(grammar::await_expression, code);
        ASSERT_TRUE(result) << result.error_message;
    }

    {
        auto code = "await act for 5sec";
        auto result = flow::parse_using(grammar::await_expression, code);
        ASSERT_TRUE(result) << result.error_message;
    }

    {
        auto code = "await  desktop_app_uninstall(ask_user_pemission = true)";
        auto result = flow::parse_using(grammar::await_expression, code);
        ASSERT_TRUE(result) << result.error_message;
    }
}

TEST(Parser, condition_block)
{
    auto code = "false {return 11;}";
    auto result = flow::parse_using(grammar::condition_block, code);
    ASSERT_TRUE(result) << result.error_message;
}

TEST(Parser, if)
{
    auto test_if = [](auto code) -> ast::IfBlock {
        auto result = flow::parse_using(grammar::if_block, code);
        if (!result) throw std::runtime_error{code + (" : " + result.error_message)};
        return result.ast;
    };

    {
        auto code = R"(
if false { return 33 ;}
)";

        ast::IfBlock result = test_if(code);
        ASSERT_TRUE(result.elif_blocks.empty());
        ASSERT_TRUE(result.else_block.empty());
    }

    {
        auto code = R"(
if false { return 33 ;}
elif 10 {return 2;}
elif 20 {raise 'toto';}
)";

        ast::IfBlock result = test_if(code);
        ASSERT_EQ(result.elif_blocks.size(), 2);
        ASSERT_TRUE(result.else_block.empty());
    }

    {
        auto code = R"(
if false { return 33 ;}
elif 10 {return 2;}
elif 20 {raise 'toto';}
else {return 58;}
)";

        ast::IfBlock result = test_if(code);
        ASSERT_EQ(result.elif_blocks.size(), 2);
        ASSERT_EQ(result.else_block.size(), 1);
    }
}

TEST(Parser, program)
{
    auto code = R"(

// Some comment
import my.package ;

/* some
   commnent
*/

my_workflow: workflow = {}
var : action = (name = 'toto', age = 58day);
;;;
)";
    auto result = flow::parse_using(grammar::program, code);

    ASSERT_TRUE(result) << result.error_message;
}

///////////////////////////////////////////////////////////////////////

TEST(Parser, debug)
{
    auto code = R"(
restart : workflow = {
    print('hello');
}
await restart;

)";
    auto result = flow::parse_using(grammar::program, code);

    ASSERT_TRUE(result) << result.error_message;
}
