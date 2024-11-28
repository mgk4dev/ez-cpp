#pragma once

#include "Expression.hpp"

namespace ez::flow::grammar {
EZ_FLOW_RULE(ast::ImportStatement, import_statement);

EZ_FLOW_RULE(ast::VariableDeclaration, variable_declaration);
EZ_FLOW_RULE(ast::AssignmentStatement, assignment_statement);

EZ_FLOW_RULE(ast::WorkflowDefinition, workflow_definition);
EZ_FLOW_RULE(ast::WorkflowStatement, workflow_statement);
EZ_FLOW_RULE(ast::IfBlock, if_block);
EZ_FLOW_RULE(ast::RepeatBlock, repeat_block);

EZ_FLOW_RULE(ast::ConditionBlock, condition_block);

EZ_FLOW_RULE(ast::DelayStatement, delay_statement);
EZ_FLOW_RULE(ast::ReturnStatement, return_statement);
EZ_FLOW_RULE(ast::RaiseStatement, raise_statement);
EZ_FLOW_RULE(ast::BreakStatement, break_statement);

struct BreakKeyword : x3::symbols<> {
    BreakKeyword() { add("break"); }
} const break_keyword;

///////////////////////////////////////

// clang-format off

const auto workflow_statements = *workflow_statement;

EZ_FLOW_DEF(import_statement) = "import" >> identifier_path;
EZ_FLOW_DEF(delay_statement)  = "delay"  >> expression;
EZ_FLOW_DEF(return_statement) = "return" >> expression;
EZ_FLOW_DEF(raise_statement)  = "raise"  >> expression;
EZ_FLOW_DEF(break_statement)  = break_keyword;

EZ_FLOW_DEF(variable_declaration)  = identifier     >> ':' >> identifier >> -('=' >> expression);
EZ_FLOW_DEF(assignment_statement) = identifier_path                      >>   '=' >> expression;

const auto scope = '{' >> workflow_statements >> '}';

EZ_FLOW_DEF(repeat_block) = "repeat" >> scope;

EZ_FLOW_DEF(condition_block) =  expression >>scope;

EZ_FLOW_DEF(if_block) =
         "if"   >> condition_block
    >> *("elif" >> condition_block)
    >> -("else" >> scope);

EZ_FLOW_DEF(workflow_statement) =
      import_statement     >> semicolons
    | workflow_definition  >> optional_semicolons
    | return_statement     >> semicolons
    | raise_statement      >> semicolons
    | delay_statement      >> semicolons
    | break_statement      >> semicolons
    | if_block             >> optional_semicolons
    | repeat_block         >> optional_semicolons
    | variable_declaration >> semicolons
    | assignment_statement >> semicolons
    | expression           >> semicolons
    ;

EZ_FLOW_DEF(workflow_definition) = identifier >> ':' >> "workflow" >> "=" >> -signature >> scope;

///////////////////////////////////////

BOOST_SPIRIT_DEFINE(
    delay_statement,
    return_statement,
    raise_statement,
    break_statement,
    import_statement,
    workflow_definition,
    variable_declaration,
    assignment_statement,
    workflow_statement,
    if_block,
    repeat_block,
    condition_block
)

// clang-format on

}  // namespace ez::flow::grammar
