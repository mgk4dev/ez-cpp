#include <ez/flow/exporter.hpp>

#include <boost/json.hpp>
#include <boost/json/src.hpp>

#include <ostream>

namespace ez::flow {

namespace json {

using Object = boost::json::object;
using Value = boost::json::value;
using Array = boost::json::array;
using Kind = boost::json::kind;
using boost::json::serialize;

using FullProgram = flow::Program<ast::Program>;

std::string_view as_str(const ast::Located& item, const FullProgram& program)
{
    return program.get_code(item);
}

template <typename T>
std::string_view as_str(const Option<T>& item, const FullProgram& program)
{
    if (item)
        return as_str(item.value(), program);
    else
        return "null";
}

///////////////////////////////////////////

Value to_json(const ast::ImportStatement& item, const FullProgram& program)
{
    Object out_object;
    out_object["import"] = as_str(item.path, program);
    return out_object;
}

Value to_json(const ast::Dictionary& item, const FullProgram& program)
{
    Object out_object;
    for (const ast::Assignment& assignement : item) {
        out_object[assignement.name] = as_str(assignement.value, program);
    }
    return out_object;
}

Value to_json(const ast::Expression& item, const FullProgram& program);

Value to_json(const ast::VariableDeclaration& item, const FullProgram& program)
{
    Object self;
    self["name"] = item.name;
    self["type"] = item.type;
    if (item.expression) self["arguments"] = to_json(item.expression.value(), program);

    Object out_object;
    out_object["object"] = std::move(self);
    return out_object;
}

Value to_json(const ast::Expression& item, const FullProgram& program)
{
    Object out_object;
    out_object["expression"] = as_str(item, program);
    return out_object;
}

Value to_json(const ast::ReturnStatement& item, const FullProgram& program)
{
    Object out_object;
    out_object["return"] = as_str(item.expression, program);
    return out_object;
}

Value to_json(const ast::RaiseStatement& item, const FullProgram& program)
{
    Object out_object;
    out_object["raise"] = as_str(item.expression, program);
    return out_object;
}

Value to_json(const ast::DelayStatement& item, const FullProgram& program)
{
    Object out_object;
    out_object["delay"] = as_str(item.expression, program);
    return out_object;
}

Value to_json(const ast::AssignmentStatement& item, const FullProgram& program)
{
    Object self;
    self["name"] = as_str(item.path, program);
    self["expression"] = as_str(item.expression, program);

    Object out_object;
    out_object["assigment"] = std::move(self);
    return out_object;
}

Value to_json(const ast::WorkflowScope& item, const FullProgram& program);

Value to_json(const ast::WorkflowDefinition& workflow, const FullProgram& program);

Value to_json(const ast::WorkflowDefinitionFwd& item, const FullProgram& program)
{
    return to_json(item.get(), program);
}

Value to_json(const ast::IfBlockFwd& item, const FullProgram& program)
{
    Object self;
    {
        Object if_json;
        if_json["condition"] = as_str(item.get().if_block.condition, program);
        if_json["statements"] = to_json(item.get().if_block.statements, program);

        self["if"] = std::move(if_json);
    }

    if (!item.get().elif_blocks.empty()) {
        Array elif_root_json;

        for (const ast::ConditionBlock& elif : item.get().elif_blocks) {
            Object elif_json;

            elif_json["condition"] = as_str(elif.condition, program);
            elif_json["statements"] = to_json(item.get().if_block.statements, program);
            elif_root_json.push_back(std::move(elif_json));
        }

        self["elif"] = std::move(elif_root_json);
    }

    if (!item.get().else_block.empty()) {
        Object else_json;
        else_json["statements"] = to_json(item.get().else_block, program);
        self["elif"] = std::move(else_json);
    }

    Object out_object;
    out_object["conditional"] = std::move(self);

    return out_object;
}

Value to_json(const ast::WorkflowScope& item, const FullProgram& program)
{
    json::Array out_object;

    for (const ast::WorkflowStatement& statement : item) {
        statement.match(
            [&](const auto& statement) { out_object.push_back(to_json(statement, program)); });
    }
    return out_object;
}

Value to_json(const ast::WorkflowDefinition& workflow, const FullProgram& program)
{
    Object self;
    self["name"] = workflow.name;
    self["signature"] = as_str(workflow.signature, program);
    self["statements"] = to_json(workflow.statements, program);

    Object out_object;
    out_object["workflow"] = self;
    return out_object;
}

Value to_json(const ast::Program& statements, const FullProgram& program)
{
    json::Array out_object;
    for (const ast::WorkflowStatement& statement : statements) {
        statement.match(
            [&](const auto& statement) { out_object.push_back(to_json(statement, program)); });
    }
    return out_object;
}

}  // namespace json

void JsonExporter::run(const Program<ast::Program>& program)
{
    ostream.get() << json::to_json(program.ast, program) << std::endl;
}

}  // namespace ez::flow
