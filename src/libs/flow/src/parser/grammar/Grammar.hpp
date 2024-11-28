#pragma once

#include "Workflow.hpp"

namespace ez::flow::grammar {
EZ_FLOW_RULE(ast::Program, program);

EZ_FLOW_DEF(program) = *workflow_statement;

BOOST_SPIRIT_DEFINE(program)

}  // namespace ez::flow::grammar
