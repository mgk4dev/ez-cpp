#pragma once

#include "expression.hpp"

namespace ez::flow::grammar {

EZ_FLOW_RULE(ast::ImportStatement, import_statement);

EZ_FLOW_RULE(ast::VariableDeclaration, variable_declaration);
EZ_FLOW_RULE(ast::AssignmentStatement, assignment_statement);

EZ_FLOW_RULE(ast::WorkflowDefinition, workflow_definition);
EZ_FLOW_RULE(ast::WorkflowStatement, workflow_statement);
EZ_FLOW_RULE(ast::IfBlock, if_block);
EZ_FLOW_RULE(ast::ConditionBlock, condition_block);

EZ_FLOW_RULE(ast::DelayStatement, delay_statement);
EZ_FLOW_RULE(ast::ReturnStatement, return_statement);
EZ_FLOW_RULE(ast::RaiseStatement, raise_statement);

///////////////////////////////////////

// clang-format off

const auto workflow_statements = *workflow_statement;

EZ_FLOW_DEF(import_statement) = "import" >> identifier_path;
EZ_FLOW_DEF(delay_statement)  = "delay"  >> expression;
EZ_FLOW_DEF(return_statement) = "return" >> expression;
EZ_FLOW_DEF(raise_statement)  = "raise"  >> expression;

EZ_FLOW_DEF(variable_declaration)  = identifier     >> ':' >> identifier >> -('=' >> expression);
EZ_FLOW_DEF(assignment_statement) = identifier_path                      >>   '=' >> expression;

const auto scope = '{' >> workflow_statements >> '}';

EZ_FLOW_DEF(condition_block) =  expression >>scope;

EZ_FLOW_DEF(if_block) =
         "if"   >> condition_block
    >> *("elif" >> condition_block)
    >> -("else" >> scope);

EZ_FLOW_DEF(workflow_statement) =
      import_statement     >> semicolons
    | variable_declaration >> semicolons
    | assignment_statement >> semicolons
    | return_statement     >> semicolons
    | raise_statement      >> semicolons
    | expression           >> semicolons
    | if_block             >> optional_semicolons
    | workflow_definition  >> optional_semicolons
    ;

EZ_FLOW_DEF(workflow_definition) = identifier >> ':' >> "workflow" >> -signature >> '=' >> scope;

///////////////////////////////////////

BOOST_SPIRIT_DEFINE(
    variable_declaration,
    assignment_statement,
    workflow_definition,
    workflow_statement,
    if_block,
    condition_block,
    delay_statement,
    return_statement,
    raise_statement,
    import_statement
)

// clang-format on

}  // namespace ez::flow::grammar
